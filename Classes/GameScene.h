/*
 * GameScene.h
 *
 *  Created on: 2014/08/06
 *      Author: takyafumin
 */

#ifndef GAMESCENE_H_
#define GAMESCENE_H_

#include "cocos2d.h"
#include "Config.h"

#define MAX_BLOCK_X 8
#define MAX_BLOCK_Y 8


/** 画像：背景 */
#define PNG_BACKGROUND "background.png"

/** 画像：ゲームオーバー */
#define PNG_GAMEOVER "gameover.png"

/** 画像：リセットボタン */
#define PNG_RESET "reset1.png"

/** 音：ブロック削除 */
#define MP3_REMOVE_BLOCK "removeBlock.mp3"

/** 時間：コマ削除アニメーション */
#define REMOVING_TIME 0.1f

/** 時間：コマ移動アニメーション */
#define MOVING_TIME 0.2f

/** キー：ハイスコア */
#define KEY_HIGHSCORE "HighScore"



// ------------------------------------
// テクスチャ画像定義
// ------------------------------------

#define FONT_RED "redFont.fnt"
#define FONT_BLUE "blueFont.fnt"
#define FONT_YELLOW "yellowFont.fnt"
#define FONT_GREEN "greenFont.fnt"
#define FONT_GRAY "grayFont.fnt"
#define FONT_WHITE "whiteFont.fnt"



class GameScene: public cocos2d::CCLayer {

protected:

    /** Tag */
    enum kTag
    {
        kTagBackground = 1,
        kTagRedLabel,
        kTagBlueLabel,
        kTagYellowLabel,
        kTagGreenLabel,
        kTagGrayLabel,
        kTagScoreLabel,
        kTagGameOver,
        kTagHighScoreLabel,
        KTagBaseBlock = 10000,
    };

    /** ZOrder */
    enum kZOrder
    {
        kZOrderBackground,
        kZOrderLabel,
        kZOrderBlock,
        kZOrderGameOver,
    };

    /**
     * コマ位置格納構造体
     */
    struct PositionIndex
    {
        PositionIndex(int x1, int y1)
        {
            x = x1;
            y = y1;
        }

        int x;
        int y;
    };


    /** 背景画像 */
    cocos2d::CCSprite* m_background;

    /** ブロック処理用 */
    float m_blockSize;
    std::map<kBlock, std::list<int> > m_blockTags;

    /** 全ブロック種別保持 */
    std::vector<kBlock> blockTypes;

    /** アニメーション実行中フラグ */
    bool m_animating;

    /** スコア保持 */
    int m_score;


    // 背景表示処理
    void showBackground();


    void initForVariables();
    void showBlock();
    cocos2d::CCPoint getPosition(int posIndexX, int posIndexY);
    int getTag(int posIndexX, int posIndexY);


    // タッチイベント
    void getTouchBlockTag(cocos2d::CCPoint touchPoint, int &tag, kBlock &blockType);
    std::list<int> getSameColorBlockTags(int baseTag, kBlock blockType);
    void removeBlock(std::list<int> blockTags, kBlock blockType);
    bool hasSameColorBlock(std::list<int> blockTagList, int searchBlockTag);


    // コマ削除アニメーション
    void removingBlock(cocos2d::CCNode* block);

    PositionIndex getPositionIndex(int tag);
    void setNewPosition1(int tag, PositionIndex posIndex);
    void searchNewPosition1(std::list<int> blocks);
    void moveBlock();

    // アニメーション処理：縦にコマを落とす
    void movingBlocksAnimation1(std::list<int> blocks);

    // アニメーション完了後処理
    void movedBlocks();

    std::map<int, bool> getExistsBlockColumn();
    void searchNewPosition2();
    void setNewPosition2(int tag, PositionIndex posIndex);

    // アニメーション処理：横にコマを詰める
    void movingBlocksAnimation2();


    // コマ数表示処理
    void showLabel();

    // ゲームオーバー判定処理
    bool existsSameBlock();

    // ハイスコア保存
    void saveHighScore();
    // ハイスコア表示
    void showHighScoreLabel();

    // コールバック：リセットボタン
    void menuResetCallback(cocos2d::CCObject* pSender);
    void showResetButton();

public:
    virtual bool init();
    static cocos2d::CCScene* scene();
    CREATE_FUNC(GameScene);

    virtual bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent);

    virtual void keyBackClicked();
    virtual void keyMenuClicked();
};

#endif /* GAMESCENE_H_ */
