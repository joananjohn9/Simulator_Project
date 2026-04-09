#include "config_loader.hpp"
#include "nlohmann/json.hpp"

#include <fstream>
#include <stdexcept>
#include <vector>

using json = nlohmann::json;

namespace {

template <typename T>
T get_required(const json& j, const std::string& key)
{
    if (!j.contains(key)) {
        throw std::runtime_error("Missing key: " + key);
    }

    try {
        return j.at(key).get<T>();
    } catch (...) {
        throw std::runtime_error("Invalid type for key: " + key);
    }
}

template <typename T>
std::optional<T> get_optional(const json& j, const std::string& key)
{
    if (!j.contains(key)) {
        return std::nullopt;
    }

    try {
        return j.at(key).get<T>();
    } catch (...) {
        throw std::runtime_error("Invalid type for key: " + key);
    }
}

} // anonymous namespace


// ---------------------------
// JSON FILE LOADER
// ---------------------------
json open_json_file(const std::string& input_json_path)
{
    std::ifstream input_json_file(input_json_path);

    if (!input_json_file) {
        throw std::runtime_error(
            "Cannot open input JSON file: " + input_json_path
        );
    }

    json input_json;
    input_json_file >> input_json;

    return input_json;
}


// ---------------------------
// PARSERS
// ---------------------------
OrderExpansionConfig parse_order_expansion(const json& order_json)
{
    OrderExpansionConfig order_expansion;

    order_expansion.enabled = get_required<bool>(order_json, "enabled");
    order_expansion.max_order = get_required<int>(order_json, "max_order");

    return order_expansion;
}

GridConfig parse_grid(const json& grid_json)
{
    GridConfig grid;

    grid.k_points = get_required<int>(grid_json, "k_points");
    grid.dt_fs = get_required<double>(grid_json, "dt_fs");
    grid.t_start_fs = get_required<double>(grid_json, "t_start_fs");
    grid.t_end_fs = get_required<double>(grid_json, "t_end_fs");

    return grid;
}

CoulombConfig parse_coulomb(const json& coulomb_json)
{
    CoulombConfig coulomb;

    if (coulomb_json.contains("enabled")) {
        coulomb.enabled = get_required<bool>(coulomb_json, "enabled");
    }

    if (coulomb_json.contains("screening")) {
        coulomb.screening = get_required<std::string>(coulomb_json, "screening");
    }

    return coulomb;
}

FieldConfig parse_field(const json& field_json)
{
    FieldConfig field;

    field.id = get_required<std::string>(field_json, "id");
    field.kind = get_required<std::string>(field_json, "kind");
    field.pulse_type = get_required<std::string>(field_json, "pulse_type");
    field.amplitude = get_required<double>(field_json, "amplitude");

    field.frequency = get_optional<double>(field_json, "frequency");
    field.duration_fs = get_optional<double>(field_json, "duration_fs");
    field.t0_fs = get_optional<double>(field_json, "t0_fs");

    field.t_on_fs = get_optional<double>(field_json, "t_on_fs");
    field.t_off_fs = get_optional<double>(field_json, "t_off_fs");

    return field;
}

OutputConfig parse_output(const json& output_json)
{
    OutputConfig output;

    output.format = get_required<std::string>(output_json, "format");
    output.observables =
        get_required<std::vector<std::string>>(output_json, "observables");

    if (output_json.contains("save_k_resolved")) {
        output.save_k_resolved =
            get_required<bool>(output_json, "save_k_resolved");
    }

    if (output_json.contains("save_macroscopic")) {
        output.save_macroscopic =
            get_required<bool>(output_json, "save_macroscopic");
    }

    return output;
}


// ---------------------------
// MAIN LOADER
// ---------------------------
EngineConfig load_engine_config(const std::string& input_json_path)
{
    EngineConfig engine_config;

    json input_json = open_json_file(input_json_path);

    engine_config.schema_version =
        get_required<std::string>(input_json, "schema_version");

    engine_config.model =
        get_required<std::string>(input_json, "model");

    engine_config.order_expansion_config =
        parse_order_expansion(input_json["order_expansion"]);

    engine_config.grid_config =
        parse_grid(input_json["grid"]);

    if (input_json.contains("interactions")) {
        InteractionConfig interactions;

        if (input_json["interactions"].contains("coulomb")) {
            interactions.coulomb =
                parse_coulomb(input_json["interactions"]["coulomb"]);
        }

        engine_config.interactions_config = interactions;
    }

    if (input_json.contains("fields")) {
        for (const auto& field_json : input_json["fields"]) {
            FieldConfig field = parse_field(field_json);

            if (field.kind == "optical") {
                if (engine_config.optical_config) {
                    throw std::runtime_error("Duplicate optical field in input JSON");
                }
                engine_config.optical_config = field;
            } else if (field.kind == "dc") {
                if (engine_config.dc_config) {
                    throw std::runtime_error("Duplicate dc field in input JSON");
                }
                engine_config.dc_config = field;
            } else {
                throw std::runtime_error("Unknown field kind: " + field.kind);
            }
        }
    }

    if (!engine_config.optical_config && !engine_config.dc_config) {
        throw std::runtime_error("No optical or dc field found in input JSON");
    }

    engine_config.output_config = parse_output(input_json["output"]);

    return engine_config;
}