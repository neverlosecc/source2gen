#include <cstdint>
#include <string>

namespace name_parser {
    struct field_info_t {
        constexpr field_info_t() = default;
        constexpr ~field_info_t() = default;
    public:
        std::string m_type = ""; // var type
        std::string m_name = ""; // var name

        // array post-fix, for example "[13][37]"
        // @todo: @soufiw: maybe make this as std::vector< int > sizes
        // and print as fmt::format( "{} {}{};\n", m_type, m_name fmt::join( sizes ) );
        std::string m_array = ""; 

        std::size_t m_bitfield_size = 0ull; // bitfield size, set to 0 if var isn't a bitfield

    public:
        __forceinline bool is_bitfield() const {
            return static_cast<bool>(m_bitfield_size);
        }

        __forceinline bool is_array() const {
            return !m_array.empty();
        }
    public:
        __forceinline std::string formatted_name() const {
            if (is_bitfield())
                return std::format("{}: {}", m_name, m_bitfield_size);

            if (is_array())
                return std::format("{}{}", m_name, m_array);

            return m_name;
        }
    };

    namespace detail {
        namespace {
            using namespace std::string_view_literals;

            constexpr std::string_view kBitfieldTypePrefix = "bitfield:"sv;

            constexpr std::string_view kArraySizePrefix = "["sv;
            constexpr std::string_view kArraySizePostfix = "]"sv;

            // clang-format off
            constexpr std::initializer_list<std::pair<std::string_view, std::string_view>> kTypeNameToCpp = {
                {"float32"sv, "float"sv}, 
                {"float64"sv, "double"sv},
    
                {"int8"sv, "int8_t"sv},   
                {"int16"sv, "int16_t"sv},   
                {"int32"sv, "int32_t"sv},   
                {"int64"sv, "int64_t"sv},
    
                {"uint8"sv, "uint8_t"sv}, 
                {"uint16"sv, "uint16_t"sv}, 
                {"uint32"sv, "uint32_t"sv}, 
                {"uint64"sv, "uint64_t"sv}
            };
            // clang-format on
        } // namespace

        // @note: @es3n1n: basically the same thing as std::atoi
        // but an exception would be thrown if we are unable to parse the string
        //
        __forceinline int wrapped_atoi(const char* s) {
            const int result = std::atoi(s);

            if (result == 0 && s && s[0] != '0')
                throw std::runtime_error(std::format("{} : Unable to parse '{}'", __FUNCTION__, s));

            return result;
        }

        __forceinline void parse_bitfield(field_info_t& result, const std::string& type_name) {
            // @note: @es3n1n: in source2 schema, every bitfield var name would start with the "bitfield:" prefix
            // so if there's no such prefix we would just skip the bitfield parsing.
            if (type_name.size() < kBitfieldTypePrefix.size())
                return;

            if (const auto s = type_name.substr(0, kBitfieldTypePrefix.size()); s != kBitfieldTypePrefix.data())
                return;

            // @note: @es3n1n: type_name starts with the "bitfield:" prefix,
            // now we can parse the bitfield size
            const auto bitfield_size_str = type_name.substr(kBitfieldTypePrefix.size(), type_name.size() - kBitfieldTypePrefix.size());
            const auto bitfield_size = wrapped_atoi(bitfield_size_str.data());

            // @note: @es3n1n: saving parsed value
            result.m_bitfield_size = bitfield_size;

            // @fixme: @es3n1n: -
            result.m_type = "uint8_t";
        }

        // @note: @es3n1n: we are assuming that this function would be executed right after
        // the bitfield/array parsing and the type would be already set if item is a bitfield
        // or array
        //
        __forceinline void parse_type(field_info_t& result, const std::string& type_name) {
            if (result.m_type.empty())
                result.m_type = type_name;

            // @note: @es3n1n: applying kTypeNameToCpp rules
            for (auto& rule : kTypeNameToCpp) {
                if (result.m_type != rule.first)
                    continue;

                result.m_type = rule.second;
                break;
            }
        }
    } // namespace detail

    inline field_info_t parse(const std::string& type_name, const std::string& name, const std::string& arrays_postfix) {
        field_info_t result = {};
        result.m_name = name;
        result.m_array = arrays_postfix;

        detail::parse_bitfield(result, type_name);
        detail::parse_type(result, type_name);

        return result;
    }
} // namespace name_parser