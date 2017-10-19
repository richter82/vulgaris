/*
 *
 * (C) 2017 - giuseppe.baccini@gmail.com
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

/*****************************************
MODEL TESTS
******************************************/
#include <iostream>
#include "vlg_model_sample.h"

using namespace vlg;
using namespace vlg_model_sample;

static void  TypeSize(FILE *f)
{
    fprintf(f, "%lu\n", sizeof(bool));
    fprintf(f, "%lu\n", sizeof(short));
    fprintf(f, "%lu\n", sizeof(unsigned short));
    fprintf(f, "%lu\n", sizeof(int));
    fprintf(f, "%lu\n", sizeof(unsigned int));
    fprintf(f, "%lu\n", sizeof(int64_t));
    fprintf(f, "%lu\n", sizeof(uint64_t));
    fprintf(f, "%lu\n", sizeof(float));
    fprintf(f, "%lu\n", sizeof(double));
    fprintf(f, "%lu\n", sizeof(char));
    fprintf(f, "%lu\n", sizeof(unsigned char));
    fprintf(f, "%lu\n", sizeof(wchar_t));
}

int main(int argc, char *argv[])
{
    entity_manager *em = get_em_vlg_model_sample();
    USER u;

    u.set_field_by_name("surname", "this is a test", strlen("this is a test"));
    u.pretty_dump_to_file(stdout);
    std::cout << std::endl;

    u.set_field_zero_by_name("surname");
    u.pretty_dump_to_file(stdout);
    std::cout << std::endl;
}
