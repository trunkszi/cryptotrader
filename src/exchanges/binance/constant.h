//
// Created by shiqj on 2022/9/8.
//

#ifndef CRYPTOTRADER_CONSTANT_H
#define CRYPTOTRADER_CONSTANT_H
namespace binance {
    /// 现货基础信息查询url,备用url: https://api1.binance.com, https://api2.binance.com, https://api3.binance.com
    constexpr const char * base_url = "https://api.binance.com";

    /// U本位基础信息查询url
    constexpr const char * base_url_futures = "https://fapi.binance.com";

    /// 币本位合约基础信息查询url
    constexpr const char * coin_base_url = "https://dapi.binance.com";

    /// 欧式期权基础信息查询url
    constexpr const char * euro_base_url = "https://eapi.binance.com";




}

#endif //CRYPTOTRADER_CONSTANT_H
