//
// Created by Quincy on 2022/9/8.
//

#ifndef CRYPTOTRADER_TYPES_H
#define CRYPTOTRADER_TYPES_H

#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <boost/json.hpp>
#include <type_traits>
#include <vector>
#include <map>


namespace binance {

    /**
     * @brief 通用json序列化工具函数, 根据类标签以及类型自动序列化对象至json字符串
     * @tparam T 是实际需要序列化的类型
     * @tparam D1 是T的成员变量公开类型和保护类型的描述
     * @tparam D2 是T的成员变量私有类型的描述
     * @tparam En 用于禁用私有成员变量的序列化以及重载
     * @param v 输出参数, 用于存储序列化后的json对象
     * @param t 输入参数, 需要序列化的对象
     */
    template<typename T,
            typename D1 = boost::describe::describe_members<T,
                    boost::describe::mod_public | boost::describe::mod_protected>,
            typename D2 = boost::describe::describe_members<T, boost::describe::mod_private>,
            typename En = std::enable_if_t<boost::mp11::mp_empty<D2>::value && !std::is_union<T>::value> >
    void tag_invoke(boost::json::value_from_tag const &, boost::json::value &v, T const &t) {
        auto &obj = v.emplace_object();
        boost::mp11::mp_for_each<D1>([&](auto D) {
            obj[D.name] = boost::json::value_from(t.*D.pointer);
        });
    }


    /**
     *  @brief 帮助函数，用于抽取json对象中的某个字段
     * */
    template<class T>
    void extract(boost::json::object const &obj, char const *name, T &value) {
        value = boost::json::value_to<T>(obj.at(name));
    }


    /**
     * @brief 通用json序列化工具函数,根据有效json字符串反序列化为对象
     * @tparam T 是实际需要序列化的类型
     * @tparam D1 是T的成员变量公开类型和保护类型的描述
     * @tparam D2 是T的成员变量私有类型的描述
     * @tparam En 用于禁用私有成员变量的序列化以及重载
     * @param v 输出参数, 用于存储序列化后的json对象
     * @param t 输入参数, 需要序列化的对象
     */
    template<class T,
            class D1 = boost::describe::describe_members<T,
                    boost::describe::mod_public | boost::describe::mod_protected>,
            class D2 = boost::describe::describe_members<T, boost::describe::mod_private>,
            class En = std::enable_if_t<boost::mp11::mp_empty<D2>::value && !std::is_union<T>::value> >
    T tag_invoke(boost::json::value_to_tag<T> const &, boost::json::value const &v) {
        auto const &obj = v.as_object();
        T t{};
        boost::mp11::mp_for_each<D1>([&](auto D) {
            extract(obj, D.name, t.*D.pointer);
        });
        return t;
    }

    struct WebsocketRequest {
        std::string method;
        std::vector<std::string> params;
        uint64_t id;
    };


}


#endif //CRYPTOTRADER_TYPES_H
