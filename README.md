# SPIKE
Backend framework for C++
### Backend Development using `C++`
#### Currently supports `windows`

```c++
  HttpServer server("3456");

  // Example of routing (Adding relative child routes)

  server.OnPath("/<...>", [](Request& req , Response& res)
  {
      res.SendString("Hello World");
  }) -> 
  addRelativeChildRoutes("/relative_child_1", [](Request& req , Response& res)
  {
      res.SendString("Hello This is relative path - 1 , You sent " + req.PATH_DATA.front());
  }) ->
  addRelativeChildRoutes("/<...>/realative_child_2", [](Request& req, Response& res)
  {
      res.SendString("Hello This is relative path - 2 , You sent " + req.PATH_DATA.back());
  });
  
  server.Serve();
```

```c++
  HttpServer server("3456");

  // Example of routing (add routes)

  server.OnPath("/", [](Request& req , Response& res)
  {
      res.SendString("Hello Home");
  });
  server.OnPath("/home_data_item/<...>", [](Request& req , Response& res)
  {
      res.SendString("Hello you requested for item:" + req.PATH_DATA.back());
  });

  server.Serve();
```
