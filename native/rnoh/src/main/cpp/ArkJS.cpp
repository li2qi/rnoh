#include "ArkJS.h"
#include "napi/native_api.h"
#include <string>

ArkJS::ArkJS(napi_env env) {
    m_env = env;
}

napi_env ArkJS::getEnv() {
    return m_env;
}

napi_value ArkJS::call(napi_value callback, std::vector<napi_value> args, napi_value thisObject) {
    napi_value result;
    auto status = napi_call_function(m_env, thisObject, callback, args.size(), args.data(), &result);
    this->maybeThrowFromStatus(status, "Couldn't call a callback");
    return result;
}

napi_value ArkJS::createBoolean(bool value) {
    napi_value result;
    napi_get_boolean(m_env, value, &result);
    return result;
}

napi_value ArkJS::createInt(int value) {
    napi_value result;
    napi_create_int32(m_env, static_cast<int32_t>(value), &result);
    return result;
}

napi_value ArkJS::createDouble(double value) {
    napi_value result;
    napi_create_double(m_env, value, &result);
    return result;
}

napi_value ArkJS::createString(std::string const &str) {
    napi_value result;
    auto status = napi_create_string_utf8(m_env, str.c_str(), str.length(), &result);
    this->maybeThrowFromStatus(status, "Failed to create string");
    return result;
}

napi_value ArkJS::getUndefined() {
    napi_value result;
    napi_get_undefined(m_env, &result);
    return result;
}

napi_value ArkJS::getNull() {
    napi_value result;
    napi_get_null(m_env, &result);
    return result;
}

RNOHNapiObjectBuilder ArkJS::createObjectBuilder() {
    return RNOHNapiObjectBuilder(m_env, *this);
}

napi_value ArkJS::getReferenceValue(napi_ref ref) {
    napi_value result;
    auto status = napi_get_reference_value(m_env, ref, &result);
    this->maybeThrowFromStatus(status, "Couldn't get a reference value");
    return result;
}

napi_ref ArkJS::createReference(napi_value value) {
    napi_ref result;
    auto status = napi_create_reference(m_env, value, 1, &result);
    this->maybeThrowFromStatus(status, "Couldn't create a reference");
    return result;
}

napi_value ArkJS::createArray() {
    napi_value result;
    napi_create_array(m_env, &result);
    return result;
}

napi_value ArkJS::createArray(std::vector<napi_value> values) {
    napi_value result;
    napi_create_array(m_env, &result);
    for (size_t i = 0; i < values.size(); i++) {
        napi_set_element(m_env, result, i, values[i]);
    }
    return result;
}

std::vector<napi_value> ArkJS::getCallbackArgs(napi_callback_info info) {
    size_t argc;
    napi_get_cb_info(m_env, info, &argc, nullptr, nullptr, nullptr);
    return getCallbackArgs(info, argc);
}

std::vector<napi_value> ArkJS::getCallbackArgs(napi_callback_info info, size_t args_count) {
    size_t argc = args_count;
    std::vector<napi_value> args(args_count, nullptr);
    napi_get_cb_info(m_env, info, &argc, args.data(), nullptr, nullptr);
    return args;
}

RNOHNapiObject ArkJS::getObject(napi_value object) {
    return RNOHNapiObject(*this, object);
}

RNOHNapiObject ArkJS::getObject(napi_ref objectRef) {
    return RNOHNapiObject(*this, this->getReferenceValue(objectRef));
}

napi_value ArkJS::getObjectProperty(napi_value object, std::string const &key) {
    return getObjectProperty(object, this->createString(key));
}

napi_value ArkJS::getObjectProperty(napi_value object, napi_value key) {
    napi_value result;
    auto status = napi_get_property(m_env, object, key, &result);
    this->maybeThrowFromStatus(status, "Failed to retrieve property from object");
    return result;
}

bool ArkJS::getBoolean(napi_value value) {
    bool result;
    auto status = napi_get_value_bool(m_env, value, &result);
    this->maybeThrowFromStatus(status, "Failed to retrieve boolean value");
    return result;
}

double ArkJS::getDouble(napi_value value) {
    double result;
    auto status = napi_get_value_double(m_env, value, &result);
    this->maybeThrowFromStatus(status, "Failed to retrieve double value");
    return result;
}

napi_value ArkJS::getArrayElement(napi_value array, uint32_t index) {
    napi_value result;
    auto status = napi_get_element(m_env, array, index, &result);
    this->maybeThrowFromStatus(status, "Failed to retrieve value at index");
    return result;
}

uint32_t ArkJS::getArrayLength(napi_value array) {
    uint32_t length;
    auto status = napi_get_array_length(m_env, array, &length);
    this->maybeThrowFromStatus(status, "Failed to read array length");
    return length;
}

std::vector<std::pair<napi_value, napi_value>> ArkJS::getObjectProperties(napi_value object) {
    napi_value propertyNames;
    auto status = napi_get_property_names(m_env, object, &propertyNames);
    this->maybeThrowFromStatus(status, "Failed to retrieve property names");
    uint32_t length = this->getArrayLength(propertyNames);
    std::vector<std::pair<napi_value, napi_value>> result;
    for (uint32_t i = 0; i < length; i++) {
        napi_value propertyName = this->getArrayElement(propertyNames, i);
        napi_value propertyValue = this->getObjectProperty(object, propertyName);
        result.emplace_back(propertyName, propertyValue);
    }
    return result;
}

std::string ArkJS::getString(napi_value value) {
    size_t length;
    napi_status status;
    status = napi_get_value_string_utf8(m_env, value, nullptr, 0, &length);
    this->maybeThrowFromStatus(status, "Failed to get the length of the string");
    std::string buffer(length, '\0');
    status = napi_get_value_string_utf8(m_env, value, buffer.data(), length + 1, &length);
    this->maybeThrowFromStatus(status, "Failed to get the string data");
    return buffer;
}

void ArkJS::maybeThrowFromStatus(napi_status status, const char *message) {
    if (status != napi_ok) {
        std::string msg_str = message;
        std::string error_code_msg_str = ". Error code: ";
        std::string status_str = std::to_string(status);
        std::string full_msg = msg_str + error_code_msg_str + status_str;
        auto c_str = full_msg.c_str();
        this->throwError(c_str);
    }
}

void ArkJS::throwError(const char *message) {
    napi_throw_error(m_env, nullptr, message);
    // stops a code execution after throwing napi_error
    throw std::runtime_error(message);
}

napi_valuetype ArkJS::getType(napi_value value) {
    napi_valuetype result;
    auto status = napi_typeof(m_env, value, &result);
    this->maybeThrowFromStatus(status, "Failed to get value type");
    return result;
}

RNOHNapiObjectBuilder::RNOHNapiObjectBuilder(napi_env env, ArkJS arkJs) : m_env(env), m_arkJs(arkJs) {
    napi_value obj;
    napi_create_object(env, &obj);
    m_object = obj;
}

RNOHNapiObjectBuilder &RNOHNapiObjectBuilder::addProperty(const char *name, napi_value value) {
    napi_set_named_property(m_env, m_object, name, value);
    return *this;
}

RNOHNapiObjectBuilder &RNOHNapiObjectBuilder::addProperty(const char *name, int value) {
    napi_set_named_property(m_env, m_object, name, m_arkJs.createInt(value));
    return *this;
}

RNOHNapiObjectBuilder &RNOHNapiObjectBuilder::addProperty(const char *name, facebook::react::Float value) {
    napi_set_named_property(m_env, m_object, name, m_arkJs.createDouble(value));
    return *this;
}

RNOHNapiObjectBuilder &RNOHNapiObjectBuilder::addProperty(const char *name, char const *value) {
    napi_set_named_property(m_env, m_object, name, m_arkJs.createString(value));
    return *this;
}

RNOHNapiObjectBuilder &RNOHNapiObjectBuilder::addProperty(const char *name, facebook::react::SharedColor value) {
    auto colorComponents = colorComponentsFromColor(value);
    napi_value n_value;
    napi_create_array(m_env, &n_value);
    napi_set_element(m_env, n_value, 0, m_arkJs.createDouble(colorComponents.red));
    napi_set_element(m_env, n_value, 1, m_arkJs.createDouble(colorComponents.green));
    napi_set_element(m_env, n_value, 2, m_arkJs.createDouble(colorComponents.blue));
    napi_set_element(m_env, n_value, 3, m_arkJs.createDouble(colorComponents.alpha));
    napi_set_named_property(m_env, m_object, name, n_value);
    return *this;
}

RNOHNapiObjectBuilder &RNOHNapiObjectBuilder::addProperty(const char *name, std::string value) {
    napi_set_named_property(m_env, m_object, name, m_arkJs.createString(value));
    return *this;
}

napi_value RNOHNapiObjectBuilder::build() {
    return m_object;
}

RNOHNapiObject::RNOHNapiObject(ArkJS arkJs, napi_value object) : m_arkJs(arkJs), m_object(object) {
}

napi_value RNOHNapiObject::getProperty(std::string const &key) {
    return m_arkJs.getObjectProperty(m_object, key);
}

napi_value RNOHNapiObject::getProperty(napi_value key) {
    return m_arkJs.getObjectProperty(m_object, key);
}

std::vector<std::pair<napi_value, napi_value>> RNOHNapiObject::getKeyValuePairs() {
    return m_arkJs.getObjectProperties(m_object);
}
