#pragma once

#include <map>
#include <string>
#include<optional>

#include "Request.h"
#include "Response.h"
#include "ParserTools.h"

class HttpRoute : public std::enable_shared_from_this<HttpRoute>
{
	using PathFunctionT = std::function<void(Request&, Response&)>;
private:
	std::map<std::string, std::shared_ptr<HttpRoute>> child_routes;
public:
	PathFunctionT path_function;
private:
	bool accepts_all = false;
public:
	HttpRoute(PathFunctionT path_function) : path_function(path_function) {}
public:
	std::pair<std::shared_ptr<HttpRoute> , std::vector<std::string>> getRelativeChildRoute(const std::string& path)
	{
		const auto routes = p_t::split_by_delms(path.begin(), path.end(), "/");
		auto shared_route = shared_from_this();
		std::vector<std::string> PATH_DATA;
		for (const auto& route : routes)
		{
			if (auto child_route_it = shared_route->child_routes.find(route) ; child_route_it != shared_route->child_routes.end()) 
			{
				shared_route = child_route_it->second;
			}
			else if ((child_route_it = shared_route->child_routes.find("<...>")) != shared_route->child_routes.end())
			{
				PATH_DATA.emplace_back(route);
				shared_route = child_route_it->second;
			}
			else if (shared_route->accepts_all)
			{
				PATH_DATA.back() += "/" + route;
			}
			else
			{
				return std::make_pair(nullptr, std::vector<std::string>());
			}
		}
		return std::make_pair(shared_route, std::move(PATH_DATA));
	}
	std::shared_ptr<HttpRoute> addRelativeChildRoutes(const std::string& path, PathFunctionT path_function)
	{
		const auto routes = p_t::split_by_delms(path.begin(), path.end(), "/");
		auto shared_route = shared_from_this();
		for (const auto& route : routes)
		{
			if (auto child_route_it = shared_route->child_routes.find(route); child_route_it != shared_route->child_routes.end())
			{
				shared_route = child_route_it->second;
			}
			else
			{
				auto child_route = std::make_shared<HttpRoute>(nullptr);
				child_route->accepts_all = route == "<...>" ;
				shared_route->child_routes[route] = child_route;
				shared_route = child_route;
			}
		}
		shared_route->path_function = path_function;
		return shared_route;
	}
};