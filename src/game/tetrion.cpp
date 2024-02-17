#include "game/tetrion.hpp"
#include "helper/constants.hpp"
#include "helper/utils.hpp"
#include "manager/recording.hpp"
#include "manager/resource_manager.hpp"
#include <cassert>
#include <fstream>
#include <spdlog/spdlog.h>
#include <sstream>

Tetrion::Tetrion(
        const u8 tetrion_index,
        const Random::Seed random_seed,
        const u32 starting_level,
        ServiceProvider* service_provider,
        helpers::optional<RecordingWriter*> recording_writer,
        const ui::Layout& layout
)
    : ui::Widget{layout},
      m_next_gravity_simulation_step_index{ get_gravity_delay_frames() },
      m_lock_delay_step_index{ lock_delay },
      m_service_provider{ service_provider },
      m_recording_writer{ recording_writer },
      m_random{ random_seed },
      m_level{ starting_level },
    m_tetrion_index{ tetrion_index },
    main_layout{
        ui::Direction::Vertical,
       std::array<double,1>{ 0.85},
       ui::AbsolutMargin{0},
        std::pair<double, double>{0.05,0.03},
               layout
        }
       {

    auto id_helper = ui::IDHelper{};

    main_layout.add<Grid>(id_helper.index());

    main_layout.add<ui::GridLayout<3>>(
            id_helper.index(), ui::Direction::Vertical, ui::AbsolutMargin{ 0 }, std::pair<double, double>{ 0.0, 0.1 }
    );


    auto* texts = get_texts();
    auto id_helper2 = ui::IDHelper{};

    constexpr auto text_size = utils::device_orientation() == utils::Orientation::Landscape
                                       ? std::pair<double, double>{ 0.2, 0.8 }
                                       : std::pair<double, double>{ 0.6, 0.8 };

    texts->add<ui::Label>(
            id_helper2.index(), service_provider, "score: 0", service_provider->fonts().get(FontId::Default),
            Color::white(), text_size, ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );


    texts->add<ui::Label>(
            id_helper2.index(), service_provider, "lines: 0", service_provider->fonts().get(FontId::Default),
            Color::white(), text_size, ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );

    texts->add<ui::Label>(
            id_helper2.index(), service_provider, "lines: 0", service_provider->fonts().get(FontId::Default),
            Color::white(), text_size, ui::Alignment{ ui::AlignmentHorizontal::Middle, ui::AlignmentVertical::Center }
    );

    refresh_texts();
}

void Tetrion::update(const SimulationStep simulation_step_index) {
    switch (m_game_state) {
        case GameState::Playing: {
            if (simulation_step_index >= m_next_gravity_simulation_step_index) {
                assert(simulation_step_index == m_next_gravity_simulation_step_index and "frame skipped?!");
                if (m_is_accelerated_down_movement and not m_down_key_pressed) {
                    assert(m_next_gravity_simulation_step_index >= get_gravity_delay_frames() and "overflow");
                    m_next_gravity_simulation_step_index -= get_gravity_delay_frames();
                    m_is_accelerated_down_movement = false;
                } else {
                    if (move_tetromino_down(
                                m_is_accelerated_down_movement ? MovementType::Forced : MovementType::Gravity,
                                simulation_step_index
                        )) {
                        reset_lock_delay(simulation_step_index);
                    }
                }
                m_next_gravity_simulation_step_index += get_gravity_delay_frames();
            }

            refresh_ghost_tetromino();
            break;
        }
        case GameState::GameOver:
        default:
            break;
    }
}

void Tetrion::render(const ServiceProvider& service_provider) const {

    main_layout.render(service_provider);

    //TODO: move the rendering into the grid, this here is ugly
    const auto* grid = get_grid();

    m_mino_stack.draw_minos(service_provider, grid);
    if (m_active_tetromino) {
        m_active_tetromino->render(service_provider, grid, MinoTransparency::Solid, Grid::grid_position);
    }
    if (m_ghost_tetromino) {
        m_ghost_tetromino->render(service_provider, grid, MinoTransparency::Ghost, Grid::grid_position);
    }
    for (std::underlying_type_t<MinoTransparency> i = 0; i < static_cast<decltype(i)>(m_preview_tetrominos.size());
         ++i) {
        if (m_preview_tetrominos.at(i)) {
            static constexpr auto enum_index = magic_enum::enum_index(MinoTransparency::Preview0);
            static_assert(enum_index.has_value());
            const auto transparency = magic_enum::enum_value<MinoTransparency>(
                    enum_index.value() + i // NOLINT(bugprone-unchecked-optional-access)
            );
            m_preview_tetrominos.at(i)->render(service_provider, grid, transparency);
        }
    }
    if (m_tetromino_on_hold) {
        m_tetromino_on_hold->render(service_provider, grid, MinoTransparency::Solid);
    }
}

[[nodiscard]] bool Tetrion::handle_event(const SDL_Event&, const Window*) {
    return false;
}

bool Tetrion::handle_input_command(const InputCommand command, const SimulationStep simulation_step_index) {
    switch (command) {
        case InputCommand::RotateLeft:
            if (rotate_tetromino_left()) {
                reset_lock_delay(simulation_step_index);
                return true;
            }
            return false;
        case InputCommand::RotateRight:
            if (rotate_tetromino_right()) {
                reset_lock_delay(simulation_step_index);
                return true;
            }
            return false;
        case InputCommand::MoveLeft:
            if (move_tetromino_left()) {
                reset_lock_delay(simulation_step_index);
                return true;
            }
            return false;
        case InputCommand::MoveRight:
            if (move_tetromino_right()) {
                reset_lock_delay(simulation_step_index);
                return true;
            }
            return false;
        case InputCommand::MoveDown:
#if not defined(__ANDROID__)
            m_down_key_pressed = true;
            m_is_accelerated_down_movement = true;
            m_next_gravity_simulation_step_index = simulation_step_index + get_gravity_delay_frames();
#endif
            if (move_tetromino_down(MovementType::Forced, simulation_step_index)) {
                reset_lock_delay(simulation_step_index);
                return true;
            }
            return false;
        case InputCommand::Drop:
            m_lock_delay_step_index = simulation_step_index; // lock instantly
            return drop_tetromino(simulation_step_index);
        case InputCommand::ReleaseMoveDown: {
            m_down_key_pressed = false;
            return false;
        }
        case InputCommand::Hold:
            if (m_allowed_to_hold) {
                hold_tetromino(simulation_step_index);
                reset_lock_delay(simulation_step_index);
                m_allowed_to_hold = false;
                return true;
            }
            return false;
        default:
            assert(false and "unknown event");
            return false;
    }
}

void Tetrion::spawn_next_tetromino(const SimulationStep simulation_step_index) {
    spawn_next_tetromino(get_next_tetromino_type(), simulation_step_index);
}

void Tetrion::spawn_next_tetromino(const TetrominoType type, const SimulationStep simulation_step_index) {
    static constexpr shapes::Point spawn_position{ 3, 0 };
    m_active_tetromino = Tetromino{ spawn_position, type };
    refresh_previews();
    if (not is_active_tetromino_position_valid()) {
        m_game_state = GameState::GameOver;

        spdlog::info("game over");
        if (m_recording_writer.has_value()) {
            spdlog::info("writing snapshot");
            (*m_recording_writer)->add_snapshot(m_tetrion_index, simulation_step_index, *this);
        }
        m_active_tetromino = {};
        m_ghost_tetromino = {};
        return;
    }
    for (usize i = 0; not is_active_tetromino_completely_visible() and i < Grid::invisible_rows; ++i) {
        m_active_tetromino->move_down();
        if (not is_active_tetromino_position_valid()) {
            m_active_tetromino->move_up();
            break;
        }
    }
    m_next_gravity_simulation_step_index = simulation_step_index + get_gravity_delay_frames();
    refresh_ghost_tetromino();
}

bool Tetrion::rotate_tetromino_right() {
    return with_lock_delay([&]() { return rotate(RotationDirection::Right); });
}

bool Tetrion::rotate_tetromino_left() {
    return with_lock_delay([&]() { return rotate(RotationDirection::Left); });
}

bool Tetrion::move_tetromino_down(MovementType movement_type, const SimulationStep simulation_step_index) {
    if (not m_active_tetromino) {
        return false;
    }
    if (movement_type == MovementType::Forced) {
        m_score += 4;
    }

    m_active_tetromino->move_down();
    if (not is_active_tetromino_position_valid()) {
        m_is_in_lock_delay = true;
        m_active_tetromino->move_up();
        if ((m_is_in_lock_delay and m_num_executed_lock_delays >= num_lock_delays)
            or simulation_step_index >= m_lock_delay_step_index) {
            lock_active_tetromino(simulation_step_index);
            reset_lock_delay(simulation_step_index);
        } else {
            m_next_gravity_simulation_step_index = simulation_step_index + 1;
        }
        return false;
    }
    return true;
}

bool Tetrion::move_tetromino_left() {
    return with_lock_delay([&]() { return move(MoveDirection::Left); });
}

bool Tetrion::move_tetromino_right() {
    return with_lock_delay([&]() { return move(MoveDirection::Right); });
}

bool Tetrion::drop_tetromino(const SimulationStep simulation_step_index) {
    if (not m_active_tetromino) {
        return false;
    }
    int num_movements = 0;
    while (is_active_tetromino_position_valid()) {
        ++num_movements;
        m_active_tetromino->move_down();
    }
    m_active_tetromino->move_up();
    m_score += 4 * num_movements;
    lock_active_tetromino(simulation_step_index);
    return num_movements > 0;
}

void Tetrion::hold_tetromino(const SimulationStep simulation_step_index) {
    if (not m_active_tetromino.has_value()) {
        return;
    }

    if (not m_tetromino_on_hold.has_value()) {
        m_tetromino_on_hold = Tetromino{ Grid::hold_tetromino_position, m_active_tetromino->type() };
        spawn_next_tetromino(simulation_step_index);
    } else {
        const auto on_hold = m_tetromino_on_hold->type();
        m_tetromino_on_hold = Tetromino{ Grid::hold_tetromino_position, m_active_tetromino->type() };
        spawn_next_tetromino(on_hold, simulation_step_index);
    }
}

void Tetrion::reset_lock_delay(const SimulationStep simulation_step_index) {
    m_lock_delay_step_index = simulation_step_index + lock_delay;
}

void Tetrion::refresh_texts() {
    auto id_helper = ui::IDHelper{};
    auto* texts = get_texts();

    std::stringstream stream;
    stream << "score: " << m_score;
    texts->get<ui::Label>(id_helper.index())->set_text(*m_service_provider, stream.str());

    stream = std::stringstream{};
    stream << "level: " << m_level;
    texts->get<ui::Label>(id_helper.index())->set_text(*m_service_provider, stream.str());

    stream = std::stringstream{};
    stream << "lines: " << m_lines_cleared;
    texts->get<ui::Label>(id_helper.index())->set_text(*m_service_provider, stream.str());
}

void Tetrion::clear_fully_occupied_lines() {
    bool cleared = false;
    const u32 lines_cleared_before = m_lines_cleared;
    do { // NOLINT(cppcoreguidelines-avoid-do-while)
        cleared = false;
        for (usize row = 0; row < Grid::height_in_tiles; ++row) {
            bool fully_occupied = true;
            for (usize column = 0; column < Grid::width_in_tiles; ++column) {
                if (m_mino_stack.is_empty(shapes::Point{ static_cast<int>(column), static_cast<int>(row) })) {
                    fully_occupied = false;
                    break;
                }
            }

            if (fully_occupied) {
                ++m_lines_cleared;
                const auto level = m_lines_cleared / 10;
                if (level > m_level) {
                    m_level = level;
                    spdlog::info("new level: {}", m_level);
                    if (level == constants::music_change_level) {
                        m_service_provider->music_manager()
                                .load_and_play_music(
                                        utils::get_assets_folder() / "music"
                                        / utils::get_supported_music_extension("03. Game Theme (50 Left)")
                                )
                                .and_then(utils::log_error);
                    }
                }
                m_mino_stack.clear_row_and_let_sink(static_cast<int>(row));
                cleared = true;
                break;
            }
        }
    } while (cleared);
    const u32 num_lines_cleared = m_lines_cleared - lines_cleared_before;
    static constexpr std::array<int, 5> score_per_line_multiplier{ 0, 40, 100, 300, 1200 };
    m_score += score_per_line_multiplier.at(num_lines_cleared) * (m_level + 1);
}

void Tetrion::lock_active_tetromino(const SimulationStep simulation_step_index) {
    assert(m_active_tetromino.has_value());
    for (const Mino& mino : m_active_tetromino->minos()) {
        m_mino_stack.set(mino.position(), mino.type());
    }
    m_allowed_to_hold = true;
    m_is_in_lock_delay = false;
    m_num_executed_lock_delays = 0;
    clear_fully_occupied_lines();
    spawn_next_tetromino(simulation_step_index);
    refresh_texts();
    reset_lock_delay(simulation_step_index);

    // save a snapshot on every freeze (only in debug builds)
#ifdef DEBUG_BUILD
    if (m_recording_writer) {
        spdlog::debug("adding snapshot at step {}", simulation_step_index);
        (*m_recording_writer)->add_snapshot(m_tetrion_index, simulation_step_index, *this);
    }
#endif
}

bool Tetrion::is_active_tetromino_position_valid() const {
    if (not m_active_tetromino) {
        return false;
    }
    return is_tetromino_position_valid(*m_active_tetromino);
}

bool Tetrion::is_valid_mino_position(shapes::Point position) const {
    return position.x >= 0 and position.x < static_cast<int>(Grid::width_in_tiles) and position.y >= 0
           and position.y < static_cast<int>(Grid::height_in_tiles) and m_mino_stack.is_empty(position);
}

bool Tetrion::is_active_tetromino_completely_visible() const {
    if (not m_active_tetromino) {
        return false;
    }
    for (const Mino& mino : m_active_tetromino->minos()) { // NOLINT(readability-use-anyofallof)
        if (mino.position().y < static_cast<int>(Grid::invisible_rows)) {
            return false;
        }
    }
    return true;
}

void Tetrion::refresh_ghost_tetromino() {
    if (not m_active_tetromino.has_value()) {
        m_ghost_tetromino = {};
        return;
    }
    m_ghost_tetromino = *m_active_tetromino;
    while (is_tetromino_position_valid(*m_ghost_tetromino)) {
        m_ghost_tetromino->move_down();
    }
    m_ghost_tetromino->move_up();
}

void Tetrion::refresh_previews() {
    auto sequence_index = m_sequence_index;
    auto bag_index = usize{ 0 };
    for (std::remove_cvref_t<decltype(num_preview_tetrominos)> i = 0; i < num_preview_tetrominos; ++i) {
        m_preview_tetrominos.at(static_cast<usize>(i)) = Tetromino{
            Grid::preview_tetromino_position + shapes::Point{0, static_cast<int>(Grid::preview_padding * i)},
            m_sequence_bags.at(bag_index)[sequence_index]
        };
        ++sequence_index;
        static constexpr auto bag_size = decltype(m_sequence_bags)::value_type::size();
        if (sequence_index >= bag_size) {
            assert(sequence_index == bag_size);
            sequence_index = 0;
            ++bag_index;
            assert(bag_index < m_sequence_bags.size());
        }
    }
}

TetrominoType Tetrion::get_next_tetromino_type() {
    const TetrominoType next_type = m_sequence_bags[0][m_sequence_index];
    m_sequence_index = (m_sequence_index + 1) % Bag::size();
    if (m_sequence_index == 0) {
        // we had a wrap-around
        m_sequence_bags[0] = m_sequence_bags[1];
        m_sequence_bags[1] = Bag{ m_random };
    }
    return next_type;
}

bool Tetrion::is_tetromino_position_valid(const Tetromino& tetromino) const {
    for (const Mino& mino : tetromino.minos()) { // NOLINT(readability-use-anyofallof)
        if (not is_valid_mino_position(mino.position())) {
            return false;
        }
    }
    return true;
}

bool Tetrion::rotate(Tetrion::RotationDirection rotation_direction) {
    if (not m_active_tetromino) {
        return false;
    }

    const auto wall_kick_table = get_wall_kick_table();
    if (not wall_kick_table.has_value()) {
        return false;
    }

    const auto from_rotation = m_active_tetromino->rotation();
    const auto to_rotation = from_rotation + (rotation_direction == RotationDirection::Left ? -1 : 1);
    const auto table_index = rotation_to_index(from_rotation, to_rotation);

    if (rotation_direction == RotationDirection::Left) {
        m_active_tetromino->rotate_left();
    } else {
        m_active_tetromino->rotate_right();
    }

    for (const auto& translation : (*wall_kick_table)->at(table_index)) {
        m_active_tetromino->move(translation);
        if (is_active_tetromino_position_valid()) {
            return true;
        }
        m_active_tetromino->move(-translation);
    }

    if (rotation_direction == RotationDirection::Left) {
        m_active_tetromino->rotate_right();
    } else {
        m_active_tetromino->rotate_left();
    }
    return false;
}

bool Tetrion::move(const Tetrion::MoveDirection move_direction) {
    if (not m_active_tetromino) {
        return false;
    }

    switch (move_direction) {
        case MoveDirection::Left:
            m_active_tetromino->move_left();
            if (not is_active_tetromino_position_valid()) {
                m_active_tetromino->move_right();
                return false;
            }
            return true;
        case MoveDirection::Right:
            m_active_tetromino->move_right();
            if (not is_active_tetromino_position_valid()) {
                m_active_tetromino->move_left();
                return false;
            }
            return true;
    }

    utils::unreachable();
}

helpers::optional<const Tetrion::WallKickTable*> Tetrion::get_wall_kick_table() const {
    assert(m_active_tetromino.has_value() and "no active tetromino");
    const auto type = m_active_tetromino->type();
    switch (type) {
        case TetrominoType::J:
        case TetrominoType::L:
        case TetrominoType::T:
        case TetrominoType::S:
        case TetrominoType::Z:
            return &wall_kick_data_jltsz;
        case TetrominoType::I:
            return &wall_kick_data_i;
        case TetrominoType::O:
            return {};
    }
    utils::unreachable();
}
