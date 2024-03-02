

local oopetris = require("oopetris")

local script_api_version : string = "1.0.0-alpha-1"

if oopetris.api_version  ~= script_api_version then
    io.stderr:write(string.format("thsi bot doesn't support the given API version, it only supports '%s' but got '%s'\n",script_api_version,oopetris))
end

-- test
oopetris.api_version = "W!"

local enum Difficulty
    "easy"
    "medium"
    "hard"
end


local instance, error : oopetris.Instance<Difficulty> | nil, string | nil= oopetris.register_instance({
    author="Totto16",
    name="example ai",
    user_options={
        difficulties={"easy", "medium", "hard"},
    }
})

if instance == nil or error ~= nil then
    local err = error or "Unknown error"
    print("Error: couldn't initialize instance: " .. err)
    return
end

local record State
    difficulty: Difficulty
    parameters: oopetris.StartParameters
end


local local_state : State = {difficulty="easy"}

local function start(start_parameters: oopetris.StartParameters, options: oopetris.InstanceUserChoiceOptions<Difficulty>)
    if options.difficulty ~= nil then
        local_state.difficulty = options.difficulty
    end

    local_state.parameters = start_parameters

    return
end

instance.on_update(start)
instance.on_late_update(start)

