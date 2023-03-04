/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include <string.h>

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief   字符串类<br>
 *           为了避免在消费者游戏机等中嵌入STL的安装。<br>
 *           std::string 的简易版多字节字符串未对应
 */
class csmString
{
public:
    /**
     * @brief   构造函数
     *
     */
    csmString();

    /**
     * @brief   参数构造函数
     *
     * @param   c   ->  コピーする文字列
     */
    csmString(const csmChar* c);

    /**
     * @brief   引数付きコンストラクタ
     *
     * @param   c       ->  文字列
     * @param   length  ->  文字列からコピーする長さ
     */
    csmString(const csmChar* c, csmInt32 length);

    /**
     * @brief   引数付きコンストラクタ
     *
     * @param[in]   s   ->  文字列
     */
    csmString(const csmString& s);

    /**
     * @brief   引数付きコンストラクタ
     *
     * @param[in]   c       ->  文字列
     * @param[in]   length  ->  文字列からコピーする長さ
     * @param[in]   usePtr  ->  trueなら引数のポインタを保持する（インスタンスと共に破棄される）<br> falseなら内部でコピーを実行する
     */
    csmString(const csmChar* c, csmInt32 length, csmBool usePtr);

    /**
     * @brief   デストラクタ
     *
     */
    virtual ~csmString();

    /**
     * @brief =演算子のオーバーロード(csmString型)
     */
    csmString& operator=(const csmString& s);

    /**
     * @brief =演算子のオーバーロード(csmChar型)
     */
    csmString& operator=(const csmChar* c);

    /**
     * @brief ==演算子のオーバーロード(csmString型)
     */
    csmBool operator==(const csmString& s) const;

    /**
     * @brief ==演算子のオーバーロード(csmChar型)
     */
    csmBool operator==(const csmChar* c) const;

    /**
     * @brief <演算子のオーバーロード(csmString型)
     */
    csmBool operator<(const csmString& s) const;

    /**
     * @brief <演算子のオーバーロード(csmChar型)
     */
    csmBool operator<(const csmChar* c) const;

    /**
     * @brief >演算子のオーバーロード(csmString型)
     */
    csmBool operator>(const csmString& s) const;

    /**
     * @brief >演算子のオーバーロード(csmChar型)
     */
    csmBool operator>(const csmChar* c) const;

    /**
     * @brief +演算子のオーバーロード(csmString型)
     */
    csmString operator+(const csmString& s) const;

    /**
     * @brief +演算子のオーバーロード(csmChar型)
     */
    csmString operator+(const csmChar* c) const;

    /**
     * @brief +=演算子のオーバーロード(csmString型)
     */
    csmString& operator+=(const csmString& s);

    /**
     * @brief +=演算子のオーバーロード(csmChar型)
     */
    csmString& operator+=(const csmChar* c);

    /**
     * @brief  文字列を後方に追加する
     *
     * @param[in]   c   ->  文字列
     * @param[in]   length  ->  追加する文字列の長さ
     * @return 更新された文字列
     */
    csmString& Append(const csmChar* c, csmInt32 length);

    /**
     * @brief   文字サイズを拡張して文字を埋める
     *
     * @param[in]   length   ->  拡張する文字数
     * @param[in]   v        ->  埋める文字
     * @return 更新された文字列
     */
    csmString& Append(csmInt32 length, const csmChar v);

    /**
     * @brief   文字列の長さを返す
     *
     * @return  文字列の長さ
     */
    csmInt32 GetLength() const { return _length; }

    /**
     * @brief   C言語文字列としてのポインタを取得する
     *
     * @return   C言語文字列のポインタ
     */
    const csmChar* GetRawString() const;

    /**
     * @brief   ポインタを解放する
     */
    void Clear();

    /**
     * @brief   ハッシュコードを取得する
     *
     * @return  ハッシュコード
     */
    csmInt32 GetHashcode();


protected:

    /**
     * @brief   文字列をコピーする. 終端に\0を追加する
     *
     * @param[in]   c   ->  文字列
     * @param[in]   length  ->  コピーする文字数
     */
    void Copy(const csmChar* c, csmInt32 length);

    /**
     * @brief   csmStringインスタンスの初期化関数。文字列のセットとハッシュコードの算出を行う。
     *
     * @param[in]   c       ->  文字列のポインタ
     * @param[in]   length  ->  文字列の長さ
     * @param[in]   usePtr  ->  trueなら第一引数のポインタを保持する(インスタンスと共に破棄される)。<br> falseなら内部でコピーを実行する
     */
    void Initialize(const csmChar* c, csmInt32 length, csmBool usePtr);

    /**
     * @brief   文字列からハッシュ値を生成して返す
     *
     * @param[in]   c       ->  文字列
     * @param[in]   length  ->  ハッシュ値の長さ
     * @return      文字列から生成したハッシュ値
     */
    csmInt32 CalcHashcode(const csmChar* c, csmInt32 length);

private:
    static const csmInt32 SmallLength = 64; ///< この長さ-1未満の文字列は内部バッファを使用
    static const csmInt32 DefaultSize = 10; ///< デフォルトの文字数
    static csmInt32 s_totalInstanceNo;      ///< 通算のインスタンス番号
    csmChar* _ptr;                          ///< 字符数组指针
    csmInt32 _length;                       ///< 半角字符数（内存确保最后为0，因此_length+1）
    csmInt32 _hashcode;                     ///< 实例的散列值
    csmInt32 _instanceNo;                   ///< 分配给实例的编号

    csmChar _small[SmallLength];            ///< 如果字符串长度小于SmallLength-1，请使用此处

    /**
     * @brief 文字列が空かどうか？
     *
     * 文字列が空かどうか確認する。
     *
     * @retval  true    空の文字列
     * @retval  false   値が設定されている
     */
    csmBool IsEmpty() const;

    /**
     * @brief 文字列を空に設定
     *
     * 文字列を空に設定する。
     *
     * @note ポインタに対してfree()はしない。
     */
    void SetEmpty();

    /**
     * @brief   C言語文字列としてのポインタを取得する
     *
     * @return   C言語文字列のポインタ
     */
    csmChar* WritePointer();
};
}}}

//------------------------- LIVE2D NAMESPACE -----------

