//
// Created by root on 2/21/24.
//

#ifndef NODE_LXC_OBJECT_H
#define NODE_LXC_OBJECT_H

#define opt_obj_val(key, getter, default) \
    (options.Has(key) ? options.Get(key).getter : default) \

#define opt_obj_strdup(key) \
    (options.Has(key) ? strdup(options.Get(key).ToString().Utf8Value().c_str()) : nullptr) \


#define opt_has_val_checked(key, getter, check) \
    (options.Has(key) && options.Get(key).checker())

#endif //NODE_LXC_OBJECT_H
