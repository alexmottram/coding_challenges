#pragma once

namespace std {

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::set <T> &s) {
	os << "{";
	
	for (const auto &itr : s) {
		os << itr << ",";
	}
	os << "}";
	return os;
}

template<typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair <T1, T2> &p) {
	os << "pair<1st:" << p.first << ", 2nd:" << p.second << ">";
	return os;
}

template<typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::map <T1, T2> &s) {
	os << "{";
	
	for (const auto &itr : s) {
		os << itr.first << ": " << itr.second << ", ";
	}
	os << "}";
	return os;
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector <T> &v) {
	for (size_t i = 0; i < v.size(); ++i) {
		if (i == 0) {
			os << "[";
		}
		os << v.at(i);
		if (i != v.size() - 1) {
			os << ", ";
		} else {
			os << "]";
		}
	}
	return os;
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::list <T> &l) {
	auto l_it = l.cbegin();
	for (size_t i = 0; i < l.size(); ++i) {
		if (i == 0) {
			os << "(";
		}
		os << *l_it;
		l_it++;
		if (i != l.size() - 1) {
			os << ", ";
		} else {
			os << ")";
		}
	}
	return os;
}

template<typename T>
std::ostream &operator<<(
		std::ostream &os,
		const std::vector <std::vector <T>> &v
) {
	
	for (size_t i = 0; i < v.size(); ++i)
		os << v.at(i) << "\n";
	return os;
}

}
