/* 
 * File:   Resource.h
 * Author: hans
 *
 * Created on 24 January 2015, 11:52
 */

#ifndef RESOURCE_H
#define	RESOURCE_H

#include "String.h"

#include <unordered_map>
#include <stdexcept>

namespace Core {

    class ResourceException : public std::runtime_error {
    public:
        ResourceException(std::string message);
    };

    template<typename Key> class ResourceNotFoundException : public ResourceException {
    public:

        ResourceNotFoundException(Key key) : ResourceException(toString("resource with key '", key, "' not found")), key_(key) {
        };

        Key key() const {
            return key_;
        };
    private:
        Key key_;
    };

    template<typename Key, typename Resource> class BasicResourcePolicy {
    protected:

        BasicResourcePolicy() {
        };

        void check(std::unordered_map<Key, const Resource *> & resources, const Key &key) {
        };

        void destroy(const Resource *resource) {
            delete resource;
        }
    };

    template<typename Key_, typename Resource_, typename ResourcePolicy = BasicResourcePolicy<Key_, Resource_> > class ResourceBundle : private ResourcePolicy {
    public:
        using Key = Key_;
        using Resource = Resource_;

        template<typename... Args> ResourceBundle(Args... args) : ResourcePolicy(args...) {
        };

        const Resource *operator[](Key key) const {
            auto found = resources_.find(key);
            if (found == resources_.end()) {
                throw ResourceNotFoundException<Key>(key);
            } else {
                return found->second;
            }
        };

        void add(Key key, const Resource *resource) {
            ResourcePolicy::check(resources_, key);
            resources_[key] = resource;
        };

        bool contains(Key key) const {
            return resources_.find(key) != resources_.end();
        };

        ~ResourceBundle() {
            for (auto i = resources_.begin(); i != resources_.end(); ++i) {
                ResourcePolicy::destroy(i->second);
            }
        };

    private:
        std::unordered_map<Key, const Resource *> resources_;

        ResourceBundle<Key_, Resource_, ResourcePolicy> &operator=(const ResourceBundle<Key_, Resource_, ResourcePolicy> &) = delete;
        ResourceBundle(const ResourceBundle<Key_, Resource_, ResourcePolicy> &) = delete;
    };

}

#endif	/* RESOURCE_H */

