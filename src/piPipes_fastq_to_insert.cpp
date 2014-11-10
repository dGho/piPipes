/*
 # piPipes, a set of pipelines for PIWI-interacting RNA (piRNA) and transposon analysis
 # Copyright (C) 2014  Bo Han, Wei Wang, Zhiping Weng, Phillip Zamore
 #
 # This program is free software; you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation; either version 3 of the License, or
 # (at your option) any later version.
 
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 
 # You should have received a copy of the GNU General Public License along
 # with this program; if not, write to the Free Software Foundation, Inc.,
 # 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <unordered_map>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>

using namespace std;

int main (int argc, char** argv)
{
	string usage=R"(
usage:
	fastqToInsert input.fq/input.fq.gz input.insert
	
	)";
	if (argc != 3)
	{
		cerr << usage ;
		exit (1);
	}
	string input_fastq_file{argv[1]};
	std::ios::sync_with_stdio(false);
	istream* p_ist_in {&std::cin};
	if (input_fastq_file != "stdin" && input_fastq_file != "-" )
	{
		p_ist_in = new std::ifstream {input_fastq_file};
	}
	if (!(*p_ist_in))
	{
		cerr << "error: cannot open file " << argv[1] << "for reading" << endl;
		exit (1);
	}
	boost::iostreams::filtering_istream in;
	char magic_number[4] = "\0\0\0";
	p_ist_in->get(magic_number, 3);
	if (
		magic_number[0] == '\037'
		&& magic_number[1] == (char)'\213'
		&& magic_number[2] == '\0'
		)
	{
		in.push(boost::iostreams::gzip_decompressor());
	}
	else if (
		magic_number[0] == 'B'
		&& magic_number[1] == 'Z'
		)
	{
		in.push(boost::iostreams::bzip2_decompressor());
	}
	else if (magic_number[0] == '@')
	{ }
	else
	{
		cerr << "unknown format" << endl;
		return 1;
	}
	p_ist_in->seekg(0, ios::beg);
	in.push(*p_ist_in);
	ofstream  out {argv[2]};
	if (!out) {
		cerr << "error: cannot open file " << argv[2] << " for writting" << endl;
		exit (1);
	}
	string line, sequence;
	unordered_map<string, int> counter;
	while (
		in.ignore    (numeric_limits<streamsize>::max(),'\n') 
		&& getline   (in, sequence) 
		&& in.ignore (numeric_limits<streamsize>::max(),'\n') 
		&& in.ignore (numeric_limits<streamsize>::max(),'\n')
		) 
	{
		++counter[sequence];
	}
	for (const auto & seq : counter) {
		out << seq.first << '\t' << seq.second << '\n';
	}
	out.close();
}



