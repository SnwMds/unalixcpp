#ifndef RULESETS
	#define RULESETS {"/storage/emulated/0/rulesets/data.min.json", "/storage/emulated/0/rulesets/unalix.json"}
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>

#include <json/json.h>

#include "uri.hpp"
#include "rulesets.hpp"
#include "utils.hpp"

/*
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
			return this.urlPattern;
		}
		
		const bool get_complete_provider() const {
			return this.completeProvider;
		}
		
		const std::vector<std::regex> get_rules() const {
			return this.rules;
		}
		
		const std::vector<std::regex> get_raw_rules() const {
			return this.rawRules;
		}
		
		const std::vector<std::regex> get_referral_marketing() const {
			return this.referralMarketing;
		}
		
		const std::vector<std::regex> get_exceptions() const {
			return this.exceptions;
		}
		
		const std::vector<std::regex> get_redirections() const {
			return this.redirections;
		}
};

std::vector<const Ruleset*> compile_rulesets() {
	
	std::vector<const Ruleset*> rulesets;
	
	for (const std::string filename : RULESETS) {
		std::ifstream file = std::ifstream(filename);
		
		if (file.fail()) {
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
*/

const std::string RULE_PREFIX = "(%(?:26|23|3[Ff])|&|\\?)";
const std::string RULE_SUFFIX = "(?:(?:=|%3[Dd])[^&]*)";

const std::string clear_url(
	const std::string url,
	const bool ignore_referral_marketing = false,
	const bool ignore_rules = false,
	const bool ignore_exceptions = false,
	const bool ignore_raw_rules = false,
	const bool ignore_redirections  = false,
	const bool skip_blocked = false
) {
	
	std::string this_url = url;
	
	for (const Ruleset ruleset: rulesets) {
		if (skip_blocked && ruleset.get_complete_provider()) {
			continue;
		}
		
		const std::regex pattern = std::regex(ruleset.get_url_pattern());
		std::smatch groups;
		
		if (std::regex_search(this_url, groups, pattern)) {
			if (!ignore_exceptions) {
				bool exception_matched = false;
				
				for (const std::string exception : ruleset.get_exceptions()) {
					const std::regex pattern = std::regex(exception);
					std::smatch groups;
					
					if (std::regex_search(this_url, groups, pattern)) {
						exception_matched = true;
						break;
					}
				}
				
				if (exception_matched) {
					continue;
				}
			}
			
			if (!ignore_redirections) {
				for (const std::string redirection : ruleset.get_redirections()) {
					const std::regex pattern = std::regex(redirection);
					std::smatch groups;
					
					if (std::regex_search(this_url, groups, pattern)) {
						const std::string target_url = groups[1];
						const std::string redirection_result = requote_uri(urldecode(target_url));
						
						// Avoid empty URLs
						if (redirection_result == "") {
							continue;
						}
						
						// Avoid duplicate URLs
						if (redirection_result == this_url) {
							continue;
						}
						
						URI uri = URI::from_string(redirection_result);
						
						// Workaround for URLs without scheme (see https://github.com/ClearURLs/Addon/issues/71)
						if (uri.get_scheme() == "") {
							uri.set_scheme("http");
						}
						
						return clear_url(
							uri.to_string(),
							ignore_referral_marketing,
							ignore_rules,
							ignore_exceptions,
							ignore_raw_rules,
							ignore_redirections,
							skip_blocked
						);
						
					}
				}
			}

			URI uri = URI::from_string(this_url);

			if (uri.get_query() != "") {
				if (!ignore_rules) {
					for (const std::string rule : ruleset.get_rules()) {
						const std::regex pattern = std::regex(RULE_PREFIX + rule + RULE_SUFFIX);
						const std::string replacement = "$1";
						const std::string query = std::regex_replace(uri.get_query(), pattern, replacement);
						
						uri.set_query(query);
					}
				}
				
				if (!ignore_referral_marketing) {
					for (const std::string referral_marketing : ruleset.get_referral_marketing()) {
						const std::regex pattern = std::regex(RULE_PREFIX + referral_marketing + RULE_SUFFIX);
						const std::string replacement = "$1";
						const std::string query = std::regex_replace(uri.get_query(), pattern, replacement);
						
						uri.set_query(query);
					}
				}
			}
			
			// The fragment might contains tracking fields as well
			if (uri.get_fragment() != "") {
				if (!ignore_rules) {
					for (const std::string rule : ruleset.get_rules()) {
						const std::regex pattern = std::regex(rule);
						const std::string replacement = "$1";
						const std::string fragment = std::regex_replace(uri.get_fragment(), pattern, replacement);
						
						uri.set_fragment(fragment);
					}
				}
				
				if (!ignore_referral_marketing) {
					for (const std::string referral_marketing : ruleset.get_referral_marketing()) {
						const std::regex pattern = std::regex(referral_marketing);
						const std::string replacement = "$1";
						const std::string fragment = std::regex_replace(uri.get_fragment(), pattern, replacement);
						
						uri.set_fragment(fragment);
					}
				}
			}
			
			if (uri.get_path() != "") {
				for (const std::string raw_rule : ruleset.get_raw_rules()) {
					const std::regex pattern = std::regex(raw_rule);
					const std::string replacement = "$1";
					const std::string path = std::regex_replace(uri.get_path(), pattern, replacement);
					
					uri.set_path(path);
				}
			}
			
			if (uri.get_query() != "") {
				uri.set_query(strip_query(uri.get_query()));
			}
			
			if (uri.get_fragment() != "") {
				uri.set_fragment(strip_query(uri.get_fragment()));
			}
			
			this_url = uri.to_string();
		}
	}
	
	return this_url;
	
}
