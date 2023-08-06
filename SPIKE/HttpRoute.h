#pragma once

#include <map>
#include <string>
#include<optional>

#include "Request.h"
#include "Response.h"

class HttpRoute : public std::enable_shared_from_this<HttpRoute>
{
	using PathFunctionT = std::function<void(Request&, Response&)>;
private:
	std::map<std::string, std::shared_ptr<HttpRoute>> child_routes;
public:
	PathFunctionT path_function;
private:
	bool accepts_all;
public:
	HttpRoute(PathFunctionT path_function, bool accepts_all = false) : path_function(path_function) , accepts_all(accepts_all) {}
public:
	std::shared_ptr<HttpRoute> getRelativeChildRoute(const std::string& path)
	{
		const auto routes = p_t::split_by_delms(path.begin(), path.end(), "/");
		auto shared_route = shared_from_this();
		for (const auto& route : routes)
		{
			if (auto child_route_it = shared_route->child_routes.find(route) ; child_route_it != shared_route->child_routes.end() 
					or 
				(child_route_it = shared_route->child_routes.find("<...>")) != shared_route->child_routes.end())
			{
				shared_route = child_route_it->second;
			}
			else if (!(shared_route->accepts_all))
			{
				return nullptr;
			}
		}
		return shared_route;
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
				route == "<...>" ? child_route->accepts_all = true : child_route->accepts_all = false;
				shared_route->child_routes[route] = child_route;
				shared_route = child_route;
			}
		}
		shared_route->path_function = path_function;
		return shared_route;
	}
};