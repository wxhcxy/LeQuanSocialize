#ifndef SNOWFLAKE_SINGLETON_H
#define SNOWFLAKE_SINGLETON_H

template <typename T>
class Singleton
{
private:
    Singleton() = default;
    ~Singleton() = default;

private:
    struct object_creator
    {
        object_creator() { Singleton<T>::instance(); }
        inline void do_nothing() const { }
    };
    static object_creator create_object;

private:
    Singleton(const Singleton&) = delete;
    Singleton(const Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(const Singleton&&) = delete;

public:
    typedef T object_type;

    static object_type& instance()
    {
        static object_type obj;

        create_object.do_nothing();

        return obj;
    }
};

template <typename T>
typename Singleton<T>::object_creator Singleton<T>::create_object;

#endif // SNOWFLAKE_SINGLETON_H
