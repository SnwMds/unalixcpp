#define PCRE2_CODE_UNIT_WIDTH 8

#ifndef RULESETS
	#define RULESETS {"../rulesets/data.min.json", "../rulesets/unalix.json"}
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>

#include <json/json.h>

#include "uri.hpp"
#include "utils.hpp"

struct Ruleset {
	private:
		const std::regex urlPattern;
		const bool completeProvider;
		const std::vector<std::regex> rules;
		const std::vector<std::regex> rawRules;
		const std::vector<std::regex> referralMarketing;
		const std::vector<std::regex> exceptions;
		const std::vector<std::regex> redirections;
	
	public:
		Ruleset(
			const std::regex urlPattern,
			const bool completeProvider,
			const std::vector<std::regex> rules,
			const std::vector<std::regex> rawRules,
			const std::vector<std::regex> referralMarketing,
			const std::vector<std::regex> exceptions,
			const std::vector<std::regex> redirections
		) :
			urlPattern(urlPattern),
			completeProvider(completeProvider),
			rules(rules),
			rawRules(rawRules),
			referralMarketing(referralMarketing),
			exceptions(exceptions),
			redirections(redirections)
		{}
		
		const std::regex get_url_pattern() const {
			return this -> urlPattern;
		}
		
		const bool get_complete_provider() const {
			return this -> completeProvider;
		}
		
		const std::vector<std::regex> get_rules() const {
			return this -> rules;
		}
		
		const std::vector<std::regex> get_raw_rules() const {
			return this -> rawRules;
		}
		
		const std::vector<std::regex> get_referral_marketing() const {
			return this -> referralMarketing;
		}
		
		const std::vector<std::regex> get_exceptions() const {
			return this -> exceptions;
		}
		
		const std::vector<std::regex> get_redirections() const {
			return this -> redirections;
		}
};

std::vector<const Ruleset*> compile_rulesets() {
	
	std::vector<const Ruleset*> rulesets;
	
	for (const std::string filename : RULESETS) {
		std::ifstream file = std::ifstream(filename);
		
		if (errno != 0) {
			fprintf(stderr, "ifstream: %s: %s\n", filename.c_str(), strerror(errno));
			abort();
		}
		
		Json::CharReaderBuilder builder;
		Json::Value root;
		JSONCPP_STRING errs;
		
		if (!parseFromStream(builder, file, &root, &errs)) {
			fprintf(stderr, "jsoncpp: %s: %s", filename.c_str(), errs.c_str());
			abort();
		}
		
		file.close();
		
		const Json::Value providers = root["providers"];
		
		for (const Json::Value name : providers.getMemberNames()) {
			const std::string provider_name = name.asString();
			
			if (provider_name.starts_with("ClearURLsTest")) {
				continue;
			}
			
			Json::Value values;
			
			const Json::Value provider = providers[provider_name];
			
			// https://docs.clearurls.xyz/latest/specs/rules/#urlpattern
			const std::regex urlPattern = std::regex(provider["urlPattern"].asString(), std::regex::optimize);
			
			// https://docs.clearurls.xyz/latest/specs/rules/#completeprovider
			const bool completeProvider = provider["completeProvider"].asBool();
			
			// https://docs.clearurls.xyz/latest/specs/rules/#rules
			values = provider["rules"];
			
			std::vector<std::regex> rules;
			
			for (int index = 0; index < values.size(); index++) {
				rules.push_back(std::regex("(%(?:26|23|3[Ff])|&|\\?)" + values[index].asString() + "(?:(?:=|%3[Dd])[^&]*)", std::regex::optimize));
			}
			
			// https://docs.clearurls.xyz/latest/specs/rules/#rawrules
			values = provider["rawRules"];
			
			std::vector<std::regex> rawRules;
			
			for (int index = 0; index < values.size(); index++) {
				rawRules.push_back(std::regex(values[index].asString(), std::regex::optimize));
			}
			
			// https://docs.clearurls.xyz/latest/specs/rules/#referralmarketing
			values = provider["referralMarketing"];
			
			std::vector<std::regex> referralMarketing;
			
			for (int index = 0; index < values.size(); index++) {
				referralMarketing.push_back(std::regex("(%(?:26|23|3[Ff])|&|\\?)" + values[index].asString() + "(?:(?:=|%3[Dd])[^&]*)", std::regex::optimize));
			}
			
			// https://docs.clearurls.xyz/latest/specs/rules/#exceptions
			values = provider["exceptions"];
			
			std::vector<std::regex> exceptions;
			
			for (int index = 0; index < values.size(); index++) {
				exceptions.push_back(std::regex(values[index].asString(), std::regex::optimize));
			}
			
			// https://docs.clearurls.xyz/latest/specs/rules/#redirections
			values = provider["redirections"];
			
			std::vector<std::regex> redirections;
			
			for (int index = 0; index < values.size(); index++) {
				redirections.push_back(std::regex(values[index].asString(), std::regex::optimize));
			}
			
			const Ruleset* ruleset = new Ruleset(
				urlPattern,
				completeProvider,
				rules,
				rawRules,
				referralMarketing,
				exceptions,
				redirections
			);
			
			rulesets.push_back(ruleset);
		}
	}
	
	return rulesets;
	
}

std::vector<const Ruleset*> rulesets;

void init_unalix() {
	
	if (rulesets.size() > 0) {
		fprintf(stderr, "init_unalix: %s\n", "rulesets object already initialized");
		abort();
	}
	
	rulesets = compile_rulesets();
	
	if (rulesets.size() < 1) {
		fprintf(stderr, "init_unalix: %s\n", "failed to initialize rulesets object");
		abort();
	}
	
}

const std::string clear_url(
	const std::string url,
	const bool ignore_referral_marketing = false,
	const bool ignore_rules = false,
	const bool ignore_exceptions = false,
	const bool ignore_raw_rules = false,
	const bool ignore_redirections  = false,
	const bool skip_blocked = false,
	const bool strip_duplicates = false,
	const bool strip_empty = false
) {
	
	std::string this_url = url;
	
	for (const Ruleset* ruleset: rulesets) {
		if (skip_blocked && ruleset -> get_complete_provider()) {
			continue;
		}
		
		std::smatch groups;
		
		if (std::regex_search(this_url, groups, ruleset -> get_url_pattern())) {
			if (!ignore_exceptions) {
				bool exception_matched = false;
				
				for (const std::regex exception : ruleset -> get_exceptions()) {
					std::smatch groups;
					
					if (std::regex_search(this_url, groups, exception)) {
						exception_matched = true;
						break;
					}
				}
				
				if (exception_matched) {
					continue;
				}
			}
			
			if (!ignore_redirections) {
				for (const std::regex redirection : ruleset -> get_redirections()) {
					std::smatch groups;
					
					if (std::regex_search(this_url, groups, redirection)) {
						const std::string target_url = groups[1];
						std::string redirection_result = requote_uri(urldecode(target_url));
						
						// Avoid empty URLs
						if (redirection_result == "") {
							continue;
						}
						
						// Avoid duplicate URLs
						if (redirection_result == this_url) {
							continue;
						}
						
						const URI* uri = URI::from_string(redirection_result.c_str());
						
						// Workaround for URLs without scheme (see https://github.com/ClearURLs/Addon/issues/71)
						if (uri -> get_scheme() == "") {
							redirection_result = "http://" + redirection_result;
						}
						
						return clear_url(
							redirection_result,
							ignore_referral_marketing,
							ignore_rules,
							ignore_exceptions,
							ignore_raw_rules,
							ignore_redirections,
							skip_blocked,
							strip_duplicates,
							strip_empty
						);
						
					}
				}
			}
			
			const URI* uri = URI::from_string(this_url);
			
			const std::string scheme = uri -> get_scheme();
			const std::string host = uri -> get_host();
			const int port = uri -> get_port();
			std::string path = uri -> get_path();
			std::string query = uri -> get_query();
			std::string fragment = uri -> get_fragment();
			
			if (query != "") {
				if (!ignore_rules) {
					for (const std::regex rule : ruleset -> get_rules()) {
						const std::string replacement = "$1";
						query = std::regex_replace(query, rule, replacement);
					}
				}
				
				if (!ignore_referral_marketing) {
					for (const std::regex referral_marketing : ruleset -> get_referral_marketing()) {
						const std::string replacement = "$1";
						query = std::regex_replace(query, referral_marketing, replacement);
					}
				}
			}
			
			// The fragment might contains tracking fields as well
			if (fragment != "") {
				if (!ignore_rules) {
					for (const std::regex rule : ruleset -> get_rules()) {
						const std::string replacement = "$1";
						fragment = std::regex_replace(fragment, rule, replacement);
					}
				}
				
				if (!ignore_referral_marketing) {
					for (const std::regex referral_marketing : ruleset -> get_referral_marketing()) {
						const std::string replacement = "$1";
						fragment = std::regex_replace(fragment, referral_marketing, replacement);
					}
				}
			}
			
			if (path != "") {
				for (const std::regex raw_rule : ruleset -> get_raw_rules()) {
					const std::string replacement = "$1";
					path = std::regex_replace(path, raw_rule, replacement);
				}
			}
			
			if (query != "") {
				query = strip_empty_r(query);
			}
			
			if (fragment != "") {
				fragment = strip_empty_r(fragment);
			}
			
			this_url = URI::to_string(scheme, host, port, path, query, fragment);
		}
	}
	
	return this_url;
	
}
