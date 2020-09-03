#pragma once

#include <linux-pe/linuxpe>

struct pe_image
{
    win::image_t<true> *image_ptr;
    std::vector<uint8_t> raw;
    std::vector<win::section_header_t> sections;
    std::vector<std::pair<uint32_t, win::reloc_entry_t>> relocations;

    pe_image() = default;
    pe_image(std::string_view name)
    {
        if (!io::read_file(name, raw))
        {
            io::log_err("failed to read image file {}.", name);
        }

        image_ptr = reinterpret_cast<win::image_t<true> *>(raw.data());

        // sections
        auto nt = image_ptr->get_nt_headers();
        for (int i = 0; i < nt->file_header.num_sections; i++)
        {
            sections.emplace_back(nt->get_section(i));
        }

        // relocations
        auto basereloc_dir = image_ptr->get_directory(win::directory_entry_basereloc);
        if (basereloc_dir->present())
        {
            auto block = image_ptr->rva_to_ptr<win::reloc_block_t>(basereloc_dir->rva);

            while (block->base_rva)
            {
                for (int i = 0; i < block->num_entries(); i++)
                {
                    auto entry = block->entries[i];
                    relocations.emplace_back(std::make_pair(block->base_rva, entry));
                }
                block = block->get_next();
            }
        }
    }

    auto operator()()
    {
        return image_ptr;
    }
};