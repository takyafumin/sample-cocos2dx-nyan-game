/*
 * Config.h
 *
 *  Created on: 2014/08/06
 *      Author: takyafumin
 */

#ifndef CONFIG_H_
#define CONFIG_H_


/**
 * ブロック
 */
enum kBlock
{
    kBlockRed,
    kBlockBlue,
    kBlockYellow,
    kBlockGreen,
    kBlockGray,
    kBlockCount,
};

/** マクロ定義：テクスチャ→文字変換 */
#define ccsf(...) CCString::createWithFormat(__VA_ARGS__)->getCString()

#endif /* CONFIG_H_ */
