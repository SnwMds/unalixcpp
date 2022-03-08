#include <string>

struct URI {
	private:
		const std::string scheme;
		const std::string host;
		const int port;
		const std::string path;
		const std::string query;
		const std::string fragment;
	public:
		URI(
			const std::string scheme,
			const std::string host,
			const int port,
			const std::string path,
			const std::string query,
			const std::string fragment
		) :
			scheme(scheme),
			host(host),
			port(port),
			path(path),
			query(query),
			fragment(fragment)
		{}
		
		const std::string get_scheme() const {
			return this -> scheme;
		}
		
		const std::string get_host() const {
			return this -> host;
		}
		
		const int get_port() const {
			return this -> port;
		}
		
		const std::string get_path() const {
			return this -> path;
		}
		
		const std::string get_query() const {
			return this -> query;
		}
		
		const std::string get_fragment() const {
			return this -> fragment;
		}
		
		static const URI* from_string(const std::string &str);
		
		static const std::string to_string(
			const std::string scheme,
			const std::string host,
			const int port,
			const std::string path,
			const std::string query,
			const std::string fragment
		);
		
};
