//============================================================================
// Name        : Directory.hpp
// Description : Course Project Part 3 (Directory Tree)
//============================================================================


#ifndef SRC_DIRECTORY_HPP_
#define SRC_DIRECTORY_HPP_

#include "TreeNode.hpp"
#include <iomanip>
#include <string>
#include <vector>

class Directory : public ensc251::TreeNode
{
	typedef std::vector<std::string> stringVect;

	private:
		std::string dir_name;
		stringVect file_names;

	public:
		Directory(std::string m_dir_name): dir_name(m_dir_name) {}
		Directory(std::string m_dir_name, stringVect m_file_names): dir_name(m_dir_name), file_names(m_file_names) {}
		virtual Directory* clone() const { return new Directory(*this); /*added*/ }

		void set_dir_name(const std::string& m_dir_name){ dir_name = m_dir_name; /* added*/ }
		const std::string & get_dir_name() { return dir_name; }
		void add_file(const std::string& m_fileName) { file_names.push_back(m_fileName); /* added*/ }
		void add_multiple_files(const stringVect& m_files) { file_names.insert(file_names.end(), m_files.begin(), m_files.end()); }
		const stringVect & get_files() const { return file_names; }

		void print_action()
		{
			// Do not modify insertion on OUT in this member function!
			OUT << std::setw(20) << dir_name+"\t|" << " ";
			OUT << this->childPVector.size() << "  ";
			for (auto file : file_names)
			{
				OUT << "  -" << file;
			}
			OUT << std::endl;
		}

		void print_traverse()
		{
			// ***** this needs work *****
			// ***** encode the rules in the instructions
			if (childPVector.size() == 2)
			{
				traverse_children_in_order(
					static_cast<traverse_func>(&Directory::print_traverse),
					static_cast<action_func>(&Directory::print_action));
			}
			else if (childPVector.size() > 2)
			{
				traverse_children_post_order(
					static_cast<traverse_func>(&Directory::print_traverse),
					static_cast<action_func>(&Directory::print_action));
			}
			else if (childPVector.size() < 2)
			{
				traverse_children_pre_order(
					static_cast<traverse_func>(&Directory::print_traverse),
					static_cast<action_func>(&Directory::print_action));
			}
		}
};

#endif /* SRC_DIRECTORY_HPP_ */