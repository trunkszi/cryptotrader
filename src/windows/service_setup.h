//
// Created by Quincy on 2022/8/30.
//

#ifndef SERVICE_SETUP_H
#define SERVICE_SETUP_H

// application
#include "boost/application/config.hpp"
#include "boost/application/base_type.hpp"
#include <boost/core/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>


namespace boost {
    namespace application {
        namespace scm {
            /**
             * @class  SCMManager
             * @brief  工具类、用于打开关闭windows注册表
             */
            class SCMManager : boost::noncopyable {
            public:
                SCMManager(DWORD dw_desired_access) {
                    boost::system::error_code ec;
                    open(dw_desired_access, ec);

                    if (ec) {
                        BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
                                "OpenSCManager() failed", ec);
                    }
                }

                SCMManager(DWORD dw_desired_access, boost::system::error_code &ec) {
                    open(dw_desired_access, ec);
                }

                ~SCMManager() {
                    CloseServiceHandle(handle_);
                }

                SC_HANDLE get() const {
                    return handle_;
                }

            protected:

                void open(DWORD dw_desired_access, boost::system::error_code &ec) {
                    // 打开注册表
                    handle_ = OpenSCManager(NULL, NULL, dw_desired_access);

                    if (handle_ == NULL) {
                        ec = last_error_code();
                    }
                }

            private:

                SC_HANDLE handle_;
            };

            /**
             * @class  CheckService
             * @brief  工具类、根据给定的名称检查服务是否存在
             */
            template<typename value_type>
            class CheckService : noncopyable {
            public:
                /// 字符类型
                using char_type = typename value_type::value_type;

                /**
                 * @method CheckService 构造函数
                 * @param service_name 服务名称
                 */
                CheckService(value_type service_name) {
                    service_name_ = service_name;
                }

                virtual ~CheckService() {
                }

                /**
                 * @method exist 服务是否存在
                 * @param ec  false情况下操作系统返回的错误码及描述信息
                 * @return true 服务存在 false 服务不存在
                 *     @retval true 服务存在
                 *     @retval false 服务不存在
                 */
                virtual bool exist(boost::system::error_code &ec) {
                    return is_installed(ec);
                }


                /**
                 * @method exist 服务是否存在
                 * @return true 服务存在 false 服务不存在
                 *     @retval true 服务存在
                 *     @retval false 服务不存在
                 */
                virtual bool exist() {
                    boost::system::error_code ec;
                    bool installed = is_installed(ec);

                    if (ec) {
                        BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
                                "is_installed() failed", ec);
                    }

                    return installed;
                }

            protected:

                /**
                * @method is_installed 服务是否已经安装
                * @param ec  false情况下操作系统返回的错误码及描述信息
                * @return true 服务存在 false 服务不存在
                *     @retval true 服务存在
                *     @retval false 服务不存在
                */
                virtual bool is_installed(boost::system::error_code &ec) {
                    SCMManager scm(SC_MANAGER_ENUMERATE_SERVICE, ec);

                    if (ec) {
                        return false;
                    }

                    ENUM_SERVICE_STATUS service;

                    DWORD dwBytesNeeded = 0;
                    DWORD dwServicesReturned = 0;
                    DWORD dwResumedHandle = 0;

                    // 查询服务
                    BOOL ret_value = EnumServicesStatus(scm.get(), SERVICE_WIN32,
                                                        SERVICE_STATE_ALL, &service, sizeof(ENUM_SERVICE_STATUS),
                                                        &dwBytesNeeded, &dwServicesReturned, &dwResumedHandle);

                    if (!ret_value) {
                        if (ERROR_MORE_DATA == GetLastError()) {

                            DWORD dwBytes = sizeof(ENUM_SERVICE_STATUS) + dwBytesNeeded;
                            ENUM_SERVICE_STATUS *pServices = NULL;
                            pServices = new ENUM_SERVICE_STATUS[dwBytes];

                            EnumServicesStatus(scm.get(), SERVICE_WIN32, SERVICE_STATE_ALL,
                                               pServices, dwBytes, &dwBytesNeeded,
                                               &dwServicesReturned, &dwResumedHandle);

                            // 遍历服务信息
                            for (unsigned iIndex = 0; iIndex < dwServicesReturned; iIndex++) {
                                std::basic_string<char_type> service;
                                service = (pServices + iIndex)->lpServiceName;

                                if (service == service_name_) {
                                    delete[] pServices;
                                    pServices = NULL;
                                    return true;
                                }
                            }

                            delete[] pServices;
                            pServices = NULL;
                        } else {
                            ec = last_error_code();
                        }
                    } else {
                        ec = last_error_code();
                    }

                    return false;
                }

                std::basic_string<char_type> service_name_;

            }; // class CheckService

            /**
             * @class  UninstallService
             * @brief  工具类、根据给定的服务名称从注册表卸载服务
             * @details 可以操作windows服务的启动、停止、删除等操作
             * @method install_windows_service 根据指定的服务名称安装服务
             */
            template<typename value_type>
            class UninstallService : boost::noncopyable {
            public:

                using char_type = typename value_type::value_type;


                UninstallService(value_type service_name, value_type service_path_name) {
                    service_name_ = service_name;
                    service_path_name_ = service_path_name;
                }

                virtual ~UninstallService() {
                }

                virtual void uninstall(boost::system::error_code &ec) {
                    uninstall_service(ec);
                }

                virtual void uninstall() {
                    boost::system::error_code ec;
                    uninstall_service(ec);

                    if (ec) {
                        BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
                                "uninstall() failed", ec);
                    }
                }

            protected:

                // 卸载服务
                void uninstall_service(boost::system::error_code &ec) {
                    boost::filesystem::path path(service_path_name_);

                    if (!boost::filesystem::exists(path)) {
                        ec = boost::system::error_code(2, boost::system::system_category());
                        return;
                    }

                    // 打开注册表
                    SCMManager scm(SC_MANAGER_CONNECT, ec);

                    if (ec) {
                        return;
                    }

                    // 打开此服务以进行 DELETE 权限操作。
                    SC_HANDLE hservice = OpenService(scm.get(), service_name_.c_str(), DELETE);

                    if (hservice == NULL) {
                        ec = last_error_code();
                        return;
                    }

                    // 从注册表数据库中删除此服务。
                    DeleteService(hservice);
                    CloseServiceHandle(hservice);
                    unregister_application(ec);
                }

                // 取消注册应用程序的应用程序路径
                void unregister_application(boost::system::error_code &ec) {
                    LONG error;

#if defined(BOOST_APPLICATION_STD_WSTRING)
                    std::basic_string<char_type> subKey =
                       L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + service_path_name_;

#else
                    std::basic_string<char_type> subKey =
                            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + service_path_name_;
#endif

                    error = RegDeleteKey(HKEY_LOCAL_MACHINE, subKey.c_str());

                    if (error != NO_ERROR) {
                        ec = last_error_code();
                    }
                }

                std::basic_string<char_type> service_name_;
                std::basic_string<char_type> service_path_name_;

            }; // uninstall_windows_service


            /**
             * @class  InstallService
             * @brief  工具类、根据给定的服务名称安装服务
             */
            template<typename value_type>
            class InstallService : noncopyable {
            public:
                using char_type = typename value_type::value_type;

                /**
                 * @method  InstallService 构造函数
                 * @brief  工具类、根据给定的服务名称安装服务
                 * @param  service_name 服务名称
                 * @param  service_display_name 服务显示名称
                 * @param  service_description 服务信息描述
                 * @param  service_path_name 服务路径名称
                 * @param  service_user 用户名、如果为空则使用系统默认用户
                 * @param  service_password 密码、如果为空则使用系统默认密码
                 * @param  service_depends 服务启动依赖的服务名称列表
                 * @param  service_option_string 服务启动参数
                 */
                InstallService(
                        value_type service_name,
                        value_type service_display_name,
                        value_type service_description,
                        value_type service_path_name,
                        value_type service_user = value_type(""),
                        value_type service_password = value_type(""),
                        value_type service_start_mode = value_type("auto"),
                        value_type service_depends = value_type(""),
                        value_type service_option_string = value_type("")) {
                    service_name_ = service_name;
                    service_display_name_ = service_display_name;
                    service_description_ = service_description;
                    service_path_name_ = service_path_name;
                    service_user_ = service_user;
                    service_password_ = service_password;
                    service_option_string_ = service_option_string;
                    service_start_mode_ = service_start_mode;
                    service_depends_ = service_depends;
                }

                virtual ~InstallService() {
                }

                virtual void install(boost::system::error_code &ec) {
                    install_service(ec);
                }

                virtual void install() {
                    boost::system::error_code ec;
                    install_service(ec);

                    if (ec) {
                        BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
                                "install() failed", ec);
                    }
                }

            protected:

                // 使用指定服务名称安装服务
                void install_service(boost::system::error_code &ec) {
                    boost::filesystem::path path(service_path_name_);

                    if (!boost::filesystem::exists(path)) {
                        ec = boost::system::error_code(2, boost::system::system_category());
                        return;
                    }

                    SCMManager scm(SC_MANAGER_CREATE_SERVICE, ec);

                    if (ec) {
                        return;
                    }

                    std::basic_string<char_type> pathname = service_path_name_;
                    if (service_option_string_.size()) {
                        pathname += char_type(' ') + service_option_string_;
                    }

                    DWORD create_service_start_mode = SERVICE_AUTO_START;
                    if (service_start_mode_ == std::basic_string<char_type>("manaul"))
                        create_service_start_mode = SERVICE_DEMAND_START;


                    std::vector<char_type> create_service_depends(service_depends_.length() + 2, 0);
                    std::copy(service_depends_.begin(), service_depends_.end(), create_service_depends.begin());
                    std::replace(create_service_depends.begin(), create_service_depends.end(), '\\', '\0');


                    // 添加至注册表数据库
                    SC_HANDLE hservice = CreateService(
                            scm.get(),
                            service_name_.c_str(),
                            service_display_name_.c_str(),
                            SERVICE_CHANGE_CONFIG,
                            SERVICE_WIN32_OWN_PROCESS,
                            create_service_start_mode,
                            SERVICE_ERROR_NORMAL,
                            pathname.c_str(),
                            NULL,
                            NULL,
                            create_service_depends.data(),
                            NULL,
                            NULL);

                    if (hservice == NULL) {
                        ec = last_error_code();
                        return;
                    }

                    bool succeed = false;

                    // 获取登陆运行权限
                    if (!service_user_.empty()) {
                        std::basic_string<char_type> actual_service_user;
                        if (service_user_.find(std::basic_string<char_type>("\\")) ==
                            std::basic_string<char_type>::npos) {
                            actual_service_user = std::basic_string<char_type>(".\\") + service_user_;
                        } else {
                            actual_service_user = service_user_;
                        }


                        succeed = ::ChangeServiceConfig(
                                hservice,                   // service handle
                                SERVICE_NO_CHANGE,          // service type
                                SERVICE_NO_CHANGE,          // start type
                                SERVICE_NO_CHANGE,          // error control
                                NULL,                       // path
                                NULL,                       // load order group
                                NULL,                       // tag id
                                NULL,                       // dependencies
                                actual_service_user.c_str(),// user account
                                service_password_.c_str(),  // user account password
                                NULL) ? true : false;                     // display name
                        // 成功 succeed = 0
                        if (!succeed) {
                            ec = last_error_code();
                            return;
                        }
                    }

                    char_type serviceDescription[2048];

#if defined(BOOST_APPLICATION_STD_WSTRING)
                    wcscpy_s(serviceDescription, sizeof(serviceDescription) / sizeof(serviceDescription[0]),
                        service_description_.c_str());
#else
                    strcpy_s(serviceDescription, sizeof(serviceDescription),
                             service_description_.c_str());
#endif

                    SERVICE_DESCRIPTION sd = {
                            serviceDescription
                    };

                    ChangeServiceConfig2(hservice, SERVICE_CONFIG_DESCRIPTION, &sd);
                    CloseServiceHandle(hservice);

                    register_application(ec);
                }

                // The App Paths registry subkeys are used to register and control the behavior
                // of the system on behalf of applications.
                // Note: if returns 5 (Access is denied) - you need run app as admin.
                void register_application(boost::system::error_code &ec) {
                    boost::filesystem::path path(service_path_name_);

                    std::basic_string<char_type> sub_key, path_entry, default_entry;

                    HKEY hkey = NULL;

                    LONG error;

#if defined(BOOST_APPLICATION_STD_WSTRING)
                    sub_key = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\"
                       + path.filename().wstring();
#else
                    sub_key = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\"
                              + path.filename().string();
#endif

                    error = RegCreateKeyEx(HKEY_LOCAL_MACHINE, sub_key.c_str(), 0, NULL,
                                           REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL,
                                           &hkey, NULL);

                    if (error != NO_ERROR) {
                        if (hkey != NULL) RegCloseKey(hkey);

                        ec = last_error_code();
                        return;
                    }

#if defined(BOOST_APPLICATION_STD_WSTRING)
                    default_entry = path.wstring();
#else
                    default_entry = path.string();
#endif

                    error = RegSetValueEx(hkey, NULL, 0, REG_EXPAND_SZ, (PBYTE)
                            default_entry.c_str(), (DWORD)default_entry.length());

                    if (error != NO_ERROR) {
                        if (hkey != NULL) RegCloseKey(hkey);
                        ec = last_error_code();
                        return;
                    }

#if defined(BOOST_APPLICATION_STD_WSTRING)
                    path_entry = path.parent_path().wstring();
#else
                    path_entry = path.parent_path().string();
#endif

                    error = RegSetValueEx(hkey, TEXT("path"), 0, REG_EXPAND_SZ,
                                          (PBYTE) path_entry.c_str(), (DWORD)path_entry.length());

                    if (error != NO_ERROR) {
                        if (hkey != NULL) RegCloseKey(hkey);
                        ec = last_error_code();
                        return;
                    }

                    if (hkey != NULL) RegCloseKey(hkey);
                }

                value_type service_name_;
                value_type service_display_name_;
                value_type service_description_;
                value_type service_path_name_;
                value_type service_option_string_;
                value_type service_start_mode_;
                value_type service_depends_;

                value_type service_user_;
                value_type service_password_;

            }; // class InstallService
        }
    }
}

#endif // SERVICE_SETUP_H


