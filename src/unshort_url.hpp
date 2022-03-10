#include <string>
#include <vector>

struct Response {
	private:
		float http_version;
		int status_code;
		std::string status_message;
		std::vector<std::tuple<std::string, std::string>> headers;
		std::string body;
	public:
		void set_http_version(const float value) {
			this -> http_version = value;
		}
		
		const float get_http_version() const {
			return this -> http_version;
		}
		
		void set_status_code(const int value) {
			this -> status_code = value;
		}
		
		const int get_status_code() const {
			return this -> status_code;
		}
		
		void set_status_message(const std::string value) {
			this -> status_message = value;
		}
		
		const std::string get_status_message() const {
			return this -> status_message;
		}
		
		void set_headers(const std::vector<std::tuple<std::string, std::string>> value) {
			this -> headers = value;
		}
		
		const std::vector<std::tuple<std::string, std::string>> get_headers() const {
			return this -> headers;
		}
		
		void set_body(const std::string value) {
			this -> body = value;
		}
		
		const std::string get_body() const {
			return this -> body;
		}
		
		const bool has_header(const std::string name) const {
			for (const std::tuple<std::string, std::string> header : this -> headers) {
				if (std::get<0>(header) == name) {
					return true;
				}
			}
			
			return false;
		}
		
		const std::string get_header(const std::string name) const {
			for (const std::tuple<std::string, std::string> header : this -> headers) {
				if (std::get<0>(header) == name) {
					return std::get<1>(header);
				}
			}
			
			return NULL;
		}
		
};