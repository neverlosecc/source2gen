#include "sdk/sdk.h"

namespace
{
	constexpr std::string_view kOutDirName = "sdk";

	constexpr std::initializer_list<std::string_view> kIncludePaths = {
		{"<cstdint>"},
		{"\"!GlobalTypes.hpp\""}
	};

	constexpr std::initializer_list<fnv32::hash> kStringMetadataEntries = {
		FNV32("MNetworkChangeCallback"),
		FNV32("MPropertyFriendlyName"),
		FNV32("MPropertyDescription"),
		FNV32("MPropertyAttributeRange"),
		FNV32("MPropertyStartGroup"),
		FNV32("MPropertyAttributeChoiceName"),
		FNV32("MPropertyGroupName"),
		FNV32("MNetworkUserGroup"),
		FNV32("MNetworkAlias"),
		FNV32("MNetworkTypeAlias"),
		FNV32("MNetworkSerializer"),
		FNV32("MPropertyAttributeEditor"),
		FNV32("MPropertySuppressExpr"),
		FNV32("MKV3TransferName"),
	};

	constexpr std::initializer_list<fnv32::hash> kIntegerMetadataEntries = {
		FNV32("MNetworkVarEmbeddedFieldOffsetDelta") ,
		FNV32("MNetworkBitCount") ,
		FNV32("MNetworkPriority") ,
		FNV32("MPropertySortPriority") ,
	};

	constexpr std::initializer_list<fnv32::hash> kFloatMetadataEntries = {
		FNV32("MNetworkMinValue"),
		FNV32("MNetworkMaxValue"),
	};

	inline bool ends_with(const std::string& str, const std::string& suffix) {
		return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
	}
}

namespace sdk
{
	CSchemaSystem* g_schema = nullptr;

	namespace
	{
		__forceinline void PrintClassInfo(codegen::generator_t::self_ref builder, std::int16_t alignment, std::int16_t size)
		{
			builder
				.comment(fmt::format("Alignment: {}", alignment))
				.comment(fmt::format("Size: {:#x}", size));
		}

		__forceinline void AssembleEnums(codegen::generator_t::self_ref builder, CUtlTSHash<CSchemaEnumBinding*> enums)
		{
			for (auto schema_enum_binding : enums.GetElements())
			{
				// @note: @es3n1n: get type name by align size
				//
				const auto get_type_name = [schema_enum_binding]() [[msvc::forceinline]]
				{
					std::string type_storage;

					switch (schema_enum_binding->m_align_)
					{
					case 1: type_storage = "uint8_t";
						break;
					case 2: type_storage = "uint16_t";
						break;
					case 4: type_storage = "uint32_t";
						break;
					case 8: type_storage = "uint64_t";
						break;
					default: type_storage = "INVALID_TYPE";
					}

					return type_storage;
				};

				// @todo: @es3n1n: assemble flags
				//
				//if (schema_enum_binding->m_flags_) out.print("// Flags: MEnumFlagsWithOverlappingBits\n");

				// @note: @es3n1n: print meta info
				//
				PrintClassInfo(builder, schema_enum_binding->m_align_, schema_enum_binding->m_size_);

				// @note: @es3n1n: begin enum class
				//
				builder.begin_enum_class(schema_enum_binding->m_binding_name_, get_type_name());

				// @note: @es3n1n: assemble enum items
				//
				for (auto l = 0; l < schema_enum_binding->m_size_; l++)
				{
					auto& field = schema_enum_binding->m_enum_info_[l];

					builder.enum_item(field.m_name, field.m_value == std::numeric_limits<std::size_t>::max() ? -1 : field.m_value);
				}

				// @note: @es3n1n: we are done with this enum
				//
				builder.end_enum_class();
			}
		}

		__forceinline void AssembleClasses(CSchemaSystemTypeScope* current, codegen::generator_t::self_ref builder, CUtlTSHash<CSchemaClassBinding*> classes)
		{
			for (const auto schema_class_binding : classes.GetElements())
			{
				// @note: @es3n1n: get class info, assemble it
				//
				const auto class_info = current->FindDeclaredClass(schema_class_binding->m_binary_name);
				const auto is_struct = ends_with(class_info->m_name, "_t");
				PrintClassInfo(builder, class_info->m_align, class_info->m_size);

				// @note: @es3n1n: get parent names
				//
				std::vector<std::string_view> parents;
				for (auto parent = class_info->m_schema_parent ? class_info->m_schema_parent->m_class : nullptr; parent;
					parent = parent->m_schema_parent ? parent->m_schema_parent->m_class : nullptr)
					parents.emplace_back(parent->m_name);

				// @note: @es3n1n: start class
				//
				if (is_struct)
					builder.begin_struct(class_info->m_name, parents);
				else
					builder.begin_class(class_info->m_name, parents);

				// @note: @es3n1n: some utils
				//
				using is_bitfield_t = bool;
				using type_name_t = std::string;
				auto get_type_info = [&](const CSchemaType* type) -> std::pair<is_bitfield_t, type_name_t>
				{
					std::string type_name = type->m_name_;

					for (const auto& [original_name, cpp_name] : s_typedefs)
						if (original_name == type_name)
							type_name = cpp_name;

					constexpr std::string_view bitfield_prefix = { "bitfield:" };
					if (type_name.size() < bitfield_prefix.size())
						return { false, type_name };

					if (auto s = type_name.substr(0, bitfield_prefix.size()); s == bitfield_prefix.data())
						return { true, type_name.substr(bitfield_prefix.size(), type_name.size() - bitfield_prefix.size()) };

					return { false, type_name };
				};

				for (auto k = 0; k < class_info->m_align; k++)
				{
					const auto field = &class_info->m_fields[k];
					if (!field)
						continue;

					// @note: @es3n1n: some more utils
					//
					auto get_metadata_type = [&](SchemaMetadataEntryData_t metadata_entry) -> std::string
					{
						std::string value;

						const auto value_hash_name = fnv32::hash_runtime(metadata_entry.m_name);

						if (std::find(kStringMetadataEntries.begin(), kStringMetadataEntries.end(), value_hash_name) != kStringMetadataEntries.end())
							value = fmt::to_string(metadata_entry.m_value->m_sz_value);
						else if (std::find(kIntegerMetadataEntries.begin(), kIntegerMetadataEntries.end(), value_hash_name) != kIntegerMetadataEntries.end())
							value = fmt::to_string(metadata_entry.m_value->m_n_value);
						else if (std::find(kFloatMetadataEntries.begin(), kFloatMetadataEntries.end(), value_hash_name) != kFloatMetadataEntries.end())
							value = fmt::to_string(metadata_entry.m_value->m_f_value);

						return value;
					};

					// @note: @es3n1n: dump metadata
					//
					for (auto j = 0; j < field->m_metadata_size; j++)
					{
						auto field_metadata = field->m_metadata[j];

						if (auto data = get_metadata_type(field_metadata); data.empty())
							builder.comment(field_metadata.m_name);
						else
							builder.comment(fmt::format("{} \"{}\"", field_metadata.m_name, data));
					}

					// @note: @es3n1n: dump prop
					//
					auto [is_bitfield, type_name] = get_type_info(field->m_type);
					builder
						.prop(is_bitfield ? "uint8_t" : type_name, is_bitfield ? fmt::format("{}:{}", field->m_name, type_name) : field->m_name, false)
						.comment(fmt::format("{:#x}", field->m_single_inheritance_offset));
				}

				// @note: @es3n1n: dump static fields
				//
				if (class_info->m_static_size)
				{
					for (auto s = 0; s < class_info->m_static_size; s++)
					{
						auto [name, m_type, m_instance, pad_0x0018] = class_info->m_static_fiels[s];

						auto [is_bitfield, type_name] = get_type_info(m_type);
						builder.static_field_getter(
							is_bitfield ? "uint8_t" : type_name, 
							is_bitfield ? fmt::format("{}:{}", name, type_name) : name, 
							current->GetScopeName().data(), 
							class_info->m_name, 
							s
						);
					}
				}

				if (!class_info->m_align)
					builder.comment("no members available");

				builder.end_block();
			}
		}
	}

	void GenerateTypeScopeSdk(CSchemaSystemTypeScope* current)
	{
		// @note: @es3n1n: getting current scope name & formatting it
		//
		constexpr std::string_view dll_extension = ".dll";
		auto                       scope_name = current->GetScopeName();
		if (ends_with(scope_name.data(), dll_extension.data())) scope_name.remove_suffix(dll_extension.size());

		// @note: @es3n1n: print debug info
		//
		fmt::print("{}: Assembling {}\n", __FUNCTION__, scope_name);

		// @note: @es3n1n: build file path
		//
		if (!std::filesystem::exists(kOutDirName))
			std::filesystem::create_directories(kOutDirName);
		const std::string out_file_path = fmt::format("{}\\{}.hpp", kOutDirName, scope_name);

		// @note: @es3n1n: init codegen
		//
		auto builder = codegen::get();
		builder.pragma("once");

		// @note: @es3n1n: include files
		//
		for (auto&& include_path : kIncludePaths)
			builder.include(include_path.data());

		// @note: @es3n1n: get stuff from schema that we'll use later
		//
		const auto current_classes = current->GetClasses();
		const auto current_enums = current->GetEnums();

		// @note: @es3n1n: print banner
		//
		builder
			.next_line()
			.comment("/////////////////////////////////////////")
			.comment(fmt::format("Binary: {}", current->GetScopeName()))
			.comment(fmt::format("Classes count: {}", current_classes.Count()))
			.comment(fmt::format("Enums count: {}", current_enums.Count()))
			.comment("/////////////////////////////////////////")
			.next_line();

		// @note: @es3n1n: assemble props
		//
		AssembleEnums(builder, current_enums);
		AssembleClasses(current, builder, current_classes);

		// @note: @es3n1n: write generated data to output file
		//
		std::ofstream f(out_file_path, std::ios::out);
		f << builder.str();
		f.close();
	}
}
