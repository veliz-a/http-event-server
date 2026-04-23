#include "router.hpp"
#include <gtest/gtest.h>

// Handler de prueba simple
HttpResponse ok_handler(const HttpRequest&) {
    return make_response(200, "{\"status\":\"ok\"}");
}

TEST(RouterTest, DispatchesCorrectHandler) {
    Router router;
    router.add("GET", "/events", ok_handler);

    HttpRequest req;
    req.method = "GET";
    req.path = "/events";

    HttpResponse res = router.dispatch(req);
    EXPECT_EQ(res.status_code, 200);
}

TEST(RouterTest, Returns404ForUnknownRoute) {
    Router router;
    router.add("GET", "/events", ok_handler);

    HttpRequest req;
    req.method = "GET";
    req.path = "/unknown";

    HttpResponse res = router.dispatch(req);
    EXPECT_EQ(res.status_code, 404);
}

TEST(RouterTest, Returns405ForWrongMethod) {
    Router router;
    router.add("GET", "/events", ok_handler);

    HttpRequest req;
    req.method = "POST";
    req.path = "/events";

    HttpResponse res = router.dispatch(req);
    EXPECT_EQ(res.status_code, 405);
}

TEST(RouterTest, DispatchesMultipleRoutes) {
    Router router;
    router.add("GET", "/events", [](const HttpRequest&) {
        return make_response(200, "{\"action\":\"list\"}");
    });
    router.add("POST", "/events", [](const HttpRequest&) {
        return make_response(201, "{\"action\":\"created\"}");
    });

    HttpRequest get_req;
    get_req.method = "GET";
    get_req.path = "/events";

    HttpRequest post_req;
    post_req.method = "POST";
    post_req.path = "/events";

    EXPECT_EQ(router.dispatch(get_req).status_code, 200);
    EXPECT_EQ(router.dispatch(post_req).status_code, 201);
}