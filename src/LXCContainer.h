//
// Created by alexander-slaa on 11/19/23.
//

#ifndef NODE_LXC_LXCCONTAINER_H
#define NODE_LXC_LXCCONTAINER_H

#include <lxc/lxccontainer.h>

class LXCContainer {
private:
    struct lxc_container *container;

public:
    explicit LXCContainer(struct lxc_container *container) : container(container) {}

    ~LXCContainer() {
        if (container) {
            // Clean up the container if needed
            lxc_container_put(container);
        }
    }

    static LXCContainer *New(napi_env env, const char *name, const char *configpath) {
        // Create the container
        struct lxc_container *container = lxc_container_new(name, configpath);
        if (!container) {
            // Handle error (e.g., container creation failed)
            Napi::TypeError::New(env, "Failed to create container").ThrowAsJavaScriptException();
        }
        return new LXCContainer(container);
    }
};

#endif //NODE_LXC_LXCCONTAINER_H
