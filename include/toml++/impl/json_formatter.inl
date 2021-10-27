//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

//# {{
#include "preprocessor.h"
#if !TOML_IMPLEMENTATION
#error This is an implementation-only header.
#endif
//# }}

#include "json_formatter.h"
#include "print_to_stream.h"
#include "table.h"
#include "array.h"
#include "header_start.h"

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	void json_formatter::print(const toml::table& tbl)
	{
		if (tbl.empty())
			impl::print_to_stream(base::stream(), "{}"sv);
		else
		{
			impl::print_to_stream(base::stream(), '{');
			if (base::indent_sub_tables())
				base::increase_indent();
			bool first = false;
			for (auto&& [k, v] : tbl)
			{
				if (first)
					impl::print_to_stream(base::stream(), ", "sv);
				first = true;
				base::print_newline(true);
				base::print_indent();

				base::print_quoted_string(k, false);
				impl::print_to_stream(base::stream(), " : "sv);

				const auto type = v.type();
				TOML_ASSUME(type != node_type::none);
				switch (type)
				{
					case node_type::table: print(*reinterpret_cast<const table*>(&v)); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
					default: base::print_value(v, type);
				}
			}
			if (base::indent_sub_tables())
				base::decrease_indent();
			base::print_newline(true);
			base::print_indent();
			impl::print_to_stream(base::stream(), '}');
		}
		base::clear_naked_newline();
	}

	TOML_EXTERNAL_LINKAGE
	void json_formatter::print(const toml::array& arr)
	{
		if (arr.empty())
			impl::print_to_stream(base::stream(), "[]"sv);
		else
		{
			impl::print_to_stream(base::stream(), '[');
			if (base::indent_array_elements())
				base::increase_indent();
			for (size_t i = 0; i < arr.size(); i++)
			{
				if (i > 0u)
					impl::print_to_stream(base::stream(), ',');
				base::print_newline(true);
				base::print_indent();

				auto& v			= arr[i];
				const auto type = v.type();
				TOML_ASSUME(type != node_type::none);
				switch (type)
				{
					case node_type::table: print(*reinterpret_cast<const table*>(&v)); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
					default: base::print_value(v, type);
				}
			}
			if (base::indent_array_elements())
				base::decrease_indent();
			base::print_newline(true);
			base::print_indent();
			impl::print_to_stream(base::stream(), ']');
		}
		base::clear_naked_newline();
	}

	TOML_EXTERNAL_LINKAGE
	void json_formatter::print()
	{
		if (base::dump_failed_parse_result())
			return;

		switch (auto source_type = base::source().type())
		{
			case node_type::table: print(*reinterpret_cast<const table*>(&base::source())); break;
			case node_type::array: print(*reinterpret_cast<const array*>(&base::source())); break;
			default: base::print_value(base::source(), source_type);
		}
	}
}
TOML_NAMESPACE_END;

#include "header_end.h"
