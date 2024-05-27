#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#if defined(__GNUC__) & !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtemplate-id-cdtor"
#endif

#include <nan.h>

#if defined(__GNUC__) & !defined(__clang__)
#pragma GCC diagnostic pop
#endif


#include <core/core.hpp>
#include <recordings/recordings.hpp>

NAN_METHOD(isRecordingFile) {

    if (info.Length() != 1) {
        info.GetIsolate()->ThrowException(Nan::TypeError("Wrong number of arguments"));
        return;
    }

    if (!info[0]->IsString()) {
        info.GetIsolate()->ThrowException(Nan::TypeError("First argument must be string"));
        return;
    }

    auto filePath = std::string{ *Nan::Utf8String(info[0]) };

    if (not std::filesystem::exists(filePath)) {

        info.GetReturnValue().Set(Nan::False());
        return;
    }

    auto parsed = recorder::RecordingReader::from_path(filePath);

    if (not parsed.has_value()) {
        info.GetReturnValue().Set(Nan::False());
        return;
    }

    info.GetReturnValue().Set(Nan::True());
    return;
}

NAN_METHOD(getInformation) {

    if (info.Length() != 1) {
        info.GetIsolate()->ThrowException(Nan::TypeError("Wrong number of arguments"));
        return;
    }

    if (!info[0]->IsString()) {
        info.GetIsolate()->ThrowException(Nan::TypeError("First argument must be string"));
        return;
    }

    auto filePath = std::string{ *Nan::Utf8String(info[0]) };

    if (not std::filesystem::exists(filePath)) {
        std::string error = "File '";
        error += filePath;
        error += "' doesn't exist!";

        info.GetIsolate()->ThrowException(Nan::Error(Nan::New(error).ToLocalChecked()));
        return;
    }

    auto parsed = recorder::RecordingReader::from_path(filePath);

    if (not parsed.has_value()) {
        std::string error = "An error occurred during parsing of the recording file '";
        error += filePath;
        error += "': ";
        error += parsed.error();

        info.GetIsolate()->ThrowException(Nan::Error(Nan::New(error).ToLocalChecked()));
        return;
    }

    const auto recording_reader = std::move(parsed.value());

    auto json_value = json::try_convert_to_json<recorder::RecordingReader>(recording_reader);

    if (not json_value.has_value()) {
        std::string error = "An error occurred during converting to json:";
        error += json_value.error();

        info.GetIsolate()->ThrowException(Nan::Error(Nan::New(error).ToLocalChecked()));
        return;
    }

    const auto result_string_json = json_value->dump(-1, ' ', false);

    v8::Local<v8::String> js_json_string = Nan::New(result_string_json).ToLocalChecked();

    Nan::JSON NanJSON;
    Nan::MaybeLocal<v8::Value> result = NanJSON.Parse(js_json_string);
    if (!result.IsEmpty()) {
        v8::Local<v8::Value> val = result.ToLocalChecked();
        info.GetReturnValue().Set(val);
        return;
    }

    info.GetIsolate()->ThrowException(Nan::Error("Failed to parse internal JSON structure!"));
    return;
}

NAN_MODULE_INIT(InitAll) {
    Nan::Set(
            target, Nan::New("isRecordingFile").ToLocalChecked(),
            Nan::GetFunction(Nan::New<v8::FunctionTemplate>(isRecordingFile)).ToLocalChecked()
    );

    Nan::Set(
            target, Nan::New("getInformation").ToLocalChecked(),
            Nan::GetFunction(Nan::New<v8::FunctionTemplate>(getInformation)).ToLocalChecked()
    );

    Nan::Set(target, Nan::New("version").ToLocalChecked(), Nan::New<v8::String>(utils::version()).ToLocalChecked());

    v8::Local<v8::Object> properties = Nan::New<v8::Object>();

    std::vector<std::pair<std::string, u32>> properties_vector{
        { "height", grid::height_in_tiles },
        {  "width",  grid::width_in_tiles }
    };

    v8::Local<v8::Object> grid_properties = Nan::New<v8::Object>();

    for (const auto& [key, value] : properties_vector) {
        v8::Local<v8::String> keyValue = Nan::New<v8::String>(key).ToLocalChecked();
        Nan::Set(grid_properties, keyValue, Nan::New<v8::Uint32>(value)).Check();
    }

    Nan::Set(properties, Nan::New<v8::String>("gridProperties").ToLocalChecked(), grid_properties).Check();

    Nan::Set(target, Nan::New("properties").ToLocalChecked(), properties);
}

NODE_MODULE(RecordingsWrapper, InitAll)
