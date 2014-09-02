/*
 * GameScene.cpp
 *
 *  Created on: 2014/08/06
 *      Author: takyafumin
 */

#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "BlockSprite.h"
#include "CCPlaySE.h"

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;


CCScene* GameScene::scene()
{
    CCScene* scene = CCScene::create();
    GameScene* layer = GameScene::create();
    scene->addChild(layer);


    return scene;
}



/**
 * 初期化処理
 */
bool GameScene::init()
{
    if(!CCLayer::init())
    {
        return false;
    }


    // タッチイベント有効化
    setTouchEnabled(true);
    setTouchMode(kCCTouchesOneByOne);

    // 物理キーイベント有効か
    setKeypadEnabled(true);


    // 変数初期化
    initForVariables();

    // 背景表示
    showBackground();

    // コマ表示
    showBlock();

    // コマ数ラベル表示
    showLabel();
    showHighScoreLabel();

    // リセットボタン表示
    showResetButton();


    // 効果音事前読込
    SimpleAudioEngine::sharedEngine()->preloadEffect(MP3_REMOVE_BLOCK);

    return true;
}


/**
 * 背景表示処理
 */
void GameScene::showBackground()
{
    // ウィンドウサイズ取得
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();

    // 背景生成
    m_background = CCSprite::create(PNG_BACKGROUND);
    m_background->setPosition(ccp(winSize.width / 2, winSize.height / 2));
    addChild(m_background, kZOrderBackground, kTagBackground);

}


/**
 * 変数初期化処理
 */
void GameScene::initForVariables()
{

    // 乱数初期化
    srand((unsigned)time(NULL));

    // コマ一辺の長さ取得
    BlockSprite* pBlock = BlockSprite::createWithBlockType(kBlockRed);
    m_blockSize = pBlock->getContentSize().height;

    // コマ種類配列の初期化
    blockTypes.push_back(kBlockRed);
    blockTypes.push_back(kBlockBlue);
    blockTypes.push_back(kBlockYellow);
    blockTypes.push_back(kBlockGreen);
    blockTypes.push_back(kBlockGray);

    // アニメーション処理中フラグ初期化
    m_animating = false;

    // スコア初期化
    m_score = 0;

    return;
}


/**
 * コマ位置取得処理
 */
CCPoint GameScene::getPosition(int posIndexX, int posIndexY)
{
    float offsetX = m_background->getContentSize().width * 0.168;
    float offsetY = m_background->getContentSize().height * 0.029;

    return CCPoint((posIndexX + 0.5) * m_blockSize + offsetX, (posIndexY + 0.5) * m_blockSize + offsetY);
}



/**
 * タグ取得処理
 */
int GameScene::getTag(int posIndexX, int posIndexY)
{
    return KTagBaseBlock + posIndexX * 100 + posIndexY;
}


/**
 * コマ表示処理
 */
void GameScene::showBlock()
{

    for (int x = 0; x < MAX_BLOCK_X; x++)
    {
        for (int y = 0; y < MAX_BLOCK_Y; y++)
        {
            // コマ種別をランダムに取得
            kBlock blockType = (kBlock)(rand() % kBlockCount);

            // 対応するコマ配列にタグを追加
            int tag = getTag(x, y);
            m_blockTags[blockType].push_back(tag);

            // コマ生成
            BlockSprite* pBlock = BlockSprite::createWithBlockType(blockType);
            pBlock->setPosition(getPosition(x, y));

            // 背景レイヤに追加
            m_background->addChild(pBlock, kZOrderBlock, tag);
        }
    }
}

/**
 * タッチイベント：開始
 */
bool GameScene::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    return true;
}


/**
 * タッチイベント：終了
 */
void GameScene::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    // タップポイント取得
    CCPoint touchPoint = m_background->convertTouchToNodeSpace(pTouch);

    // タップしたコマのTagとコマの種類を取得
    int tag = 0;
    kBlock blockType;
    getTouchBlockTag(touchPoint, tag, blockType);

    if (tag != 0)
    {
        // 隣接するコマを検索する
        list<int> sameColorBlockTags = getSameColorBlockTags(tag, blockType);
        if (sameColorBlockTags.size() > 1)
        {

            // スコア計上
            m_score += pow(sameColorBlockTags.size() - 2, 2);

            // アニメーション開始
            m_animating = true;

            // 隣接するコマを削除
            removeBlock(sameColorBlockTags, blockType);

            // コマ削除のアニメーション
            movingBlocksAnimation1(sameColorBlockTags);
        }
    }
}


/**
 * タッチしたブロックタグ取得処理
 *
 * @param touchPoint タッチしたPOINT
 * @param tag <戻り値> タッチしたブロックのタグ
 * @param blockType <戻り値> タッチしたブロックの種類
 */
void GameScene::getTouchBlockTag(CCPoint touchPoint, int &tag, kBlock &blockType)
{
    for (int x = 0; x < MAX_BLOCK_X; x++)
    {
        for (int y = 0; y < MAX_BLOCK_Y; y++)
        {
            // 対象コマのタグを取得
            int currentTag = getTag(x, y);

            // タグからコマを取得
            CCNode* node = m_background->getChildByTag(currentTag);

            if (node && node->boundingBox().containsPoint(touchPoint))
            {
                // タグの保持
                tag = currentTag;

                // ブロックタイプの保持
                blockType = ((BlockSprite*)node)->getBlockType();
                return;
            }
        }
    }
}


/**
 * コマ配列内存在チェック処理
 *
 * @param blockTagList チェック対象コマ配列
 * @param searchBlockTag チェック対象コマのタグ
 * @return 実行結果 true：一致タグあり、false：一致タグなし
 */
bool GameScene::hasSameColorBlock(list<int> blockTagList, int searchBlockTag)
{
    list<int>::iterator it;
    for (it = blockTagList.begin(); it != blockTagList.end(); ++it)
    {
        if (*it == searchBlockTag)
        {
            return true;
        }
    }

    return false;
}


/**
 * 隣接同一コマ取得
 *
 * @param baseTag 隣接チェックの起点となるコマのタグ
 * @param blockType 隣接チェックの起点となるコマの種類
 * @return 隣接同一コマのタグリスト
 */
list<int> GameScene::getSameColorBlockTags(int baseTag, kBlock blockType)
{
    // 同色のコマを格納する配列の初期化
    list<int> sameColorBlockTags;
    sameColorBlockTags.push_back(baseTag);

    list<int>::iterator it = sameColorBlockTags.begin();
    while (it != sameColorBlockTags.end())
    {
        int tags[] = {
                *it + 100,  // 右ブロック
                *it - 100,  // 左ブロック
                *it + 1,    // 上ブロック
                *it - 1,    // 下ブロック
        };

        for (int i = 0; i < sizeof(tags) / sizeof(tags[0]); i++)
        {
            // すでにリストにあるかチェック
            if (!hasSameColorBlock(sameColorBlockTags, tags[i]))
            {
                // コマ配列にあるかチェック
                if (hasSameColorBlock(m_blockTags[blockType], tags[i]))
                {
                    sameColorBlockTags.push_back(tags[i]);
                }
            }
        };

        it++;
    }


    return sameColorBlockTags;
}


/**
 * コマ削除処理
 *
 * @param blocktags 削除対象コマのリスト
 * @param blockType コマの種類
 */
void GameScene::removeBlock(list<int> blockTags, kBlock blockType)
{
    bool first = true;

     list<int>::iterator it = blockTags.begin();
     while (it != blockTags.end())
     {
         // 既存配列から該当コマを削除
         m_blockTags[blockType].remove(*it);

         // 対象となるコマを取得
         CCNode* block = m_background->getChildByTag(*it);
         if (block)
         {
             // コマが消えるアニメーション生成
             CCScaleTo* scale = CCScaleTo::create(REMOVING_TIME, 0);

             // コマを削除するアクションを生成
             CCCallFuncN* func = CCCallFuncN::create(this, callfuncN_selector(GameScene::removingBlock));

             // アクションをつなげる
             CCFiniteTimeAction* sequence = CCSequence::create(scale, func, NULL);


             CCFiniteTimeAction* action;
             if (first)
             {
                 // コマが消えるサウンドアクション生成
                 CCPlaySE* playSe = CCPlaySE::create(MP3_REMOVE_BLOCK);

                 // アクションをつなげる
                 action = CCSpawn::create(sequence, playSe, NULL);

                 first = false;
             }
             else
             {
                 action = sequence;
             }

             // アクションをセットする
             //block->removeFromParentAndCleanup(true);
             block->runAction(action);
         }

         it++;
     }

     // 効果音
     SimpleAudioEngine::sharedEngine()->playEffect(MP3_REMOVE_BLOCK);
}


/**
 * コマの削除処理
 *
 * @param 削除対象コマのノード
 */
void GameScene::removingBlock(CCNode* block)
{
    block->removeFromParentAndCleanup(true);
}


/**
 * コマインデックスの取得処理
 *
 * @param tag インデックス取得対象コマのタグ
 */
GameScene::PositionIndex GameScene::getPositionIndex(int tag)
{
    int pos1_x = (tag - KTagBaseBlock) / 100;
    int pos1_y = (tag -KTagBaseBlock) % 100;

    return PositionIndex(pos1_x, pos1_y);
}


/**
 * 新しいコマの座標設定処理：縦に詰める場合
 *
 * @param tag 対象コマのタグ
 * @param posIndex 対象コマの現在位置座標
 */
void GameScene::setNewPosition1(int tag, PositionIndex posIndex)
{
    BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(tag);
    int nextPosY = blockSprite->getNextPosY();
    if (nextPosY == -1)
    {
        nextPosY = posIndex.y;
    }

    // 移動先の位置をセット
    blockSprite->setNextPos(posIndex.x, --nextPosY);
}


void GameScene::searchNewPosition1(list<int> blocks)
{
    // 消えるコマ数分のループ
    list<int>::iterator it1 = blocks.begin();
    while (it1 != blocks.end())
    {
        PositionIndex posIndex1 = getPositionIndex(*it1);

        // コマの種類分ループ
        vector<kBlock>::iterator it2 = blockTypes.begin();
        while (it2 != blockTypes.end())
        {
            // 各種類のコマ数分ループ
            list<int>::iterator it3 = m_blockTags[*it2].begin();
            while (it3 != m_blockTags[*it2].end())
            {
                PositionIndex posIndex2 = getPositionIndex(*it3);

                if (posIndex1.x == posIndex2.x && posIndex1.y < posIndex2.y)
                {
                    // 消えるコマの上に位置するコマに対して、移動先の位置をセットする
                    setNewPosition1(*it3, posIndex2);
                }

                it3++;
            }

            it2++;

        }

        it1++;
    }
}


/**
 * コマ移動処理
 */
void GameScene::moveBlock()
{

    // コマの種類分ループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end())
    {
        // 各種類のコマ数分ループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end())
        {
            BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(*it2);

            int nextPosX = blockSprite->getNextPosX();
            int nextPosY = blockSprite->getNextPosY();

            if (nextPosX != -1 && nextPosY != -1)
            {
                // 新しいタグをセット
                int newTag = getTag(nextPosX, nextPosY);
                blockSprite->initNextPos();
                blockSprite->setTag(newTag);

                // タグ一覧の値も新しいタグに変更する
                *it2 = newTag;

                // アニメーションを設定
                CCMoveTo* move = CCMoveTo::create(MOVING_TIME, getPosition(nextPosX, nextPosY));
                blockSprite->runAction(move);
            }

            it2++;
        }

        it1++;
    }
}


/**
 * アニメーション処理：コマ削除後
 */
void GameScene::movingBlocksAnimation1(list<int> blocks)
{
    // コマの新しい位置をセットする
    searchNewPosition1(blocks);

    // 新しい位置がセットされたコマのアニメ―ション
    moveBlock();

    // アニメーション終了時に次のコマ移動処理を呼出し
    scheduleOnce(schedule_selector(GameScene::movingBlocksAnimation2), MOVING_TIME);

    return;
}


/**
 * アニメーション終了後処理
 */
void GameScene::movedBlocks()
{
    // コマ数ラベル再表示
    showLabel();

    // アニメーション処理終了
    m_animating = false;

    // ゲーム終了チェック
    if (!existsSameBlock())
    {
        // ハイスコア保存、表示
        saveHighScore();


        CCSize bgSize = m_background->getContentSize();

        // ゲーム終了表示
        CCSprite* gameOver = CCSprite::create(PNG_GAMEOVER);
        gameOver->setPosition(ccp(bgSize.width / 2, bgSize.height * 0.8));
        m_background->addChild(gameOver, kZOrderGameOver, kTagGameOver);

        setTouchEnabled(false);
    }

    return;
}


/**
 * 新しいコマの座標設定処理：横に詰める場合
 *
 * @param tag 対象コマのタグ
 * @param posIndex 対象コマの現在位置座標
 */
void GameScene::setNewPosition2(int tag, PositionIndex posIndex)
{
    // tagをもとにコマのスプライトを取得
    BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(tag);
    int nextPosX = blockSprite->getNextPosX();
    if (nextPosX == -1)
    {
        nextPosX = posIndex.x;
    }

    // 移動先の位置をセット
    blockSprite->setNextPos(--nextPosX, posIndex.y);

    return;
}

/**
 * 存在するコマをマップ情報として返却する
 */
map<int, bool> GameScene::getExistsBlockColumn()
{
    // 検索配列初期あk
    map<int, bool> xBlock;
    for (int i = 0; i < MAX_BLOCK_X; i++)
    {
        xBlock[i] = false;
    }


    // コマの種類分ループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end())
    {
        // 各種類のコマ数分ループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end())
        {
            // 存在するコマのx位置を保持
            xBlock[getPositionIndex(*it2).x] = true;

            it2++;
        }

        it1++;
    }

    return xBlock;
}


void GameScene::searchNewPosition2()
{
    // コマ存在マップ取得
    map<int, bool> xBlock = getExistsBlockColumn();

    //　コマが存在しないx位置を埋める
    bool first = true;
    for (int i = MAX_BLOCK_X - 1; i >= 0; i--)
    {
        if (xBlock[i])
        {
            // コマが存在する場合
            first = false;
            continue;
        }
        else
        {
            // コマが存在しない場合
            if (first)
            {
                // 右側にコマがない
                continue;
            }
            else
            {
                // 現在位置より右側にあるコマを左に１つ詰める

                // コマの種類分ループ
                vector<kBlock>::iterator it1 = blockTypes.begin();
                while (it1 != blockTypes.end())
                {
                    // 各種類のコマ数分ループ
                    list<int>::iterator it2 = m_blockTags[*it1].begin();
                    while (it2 != m_blockTags[*it1].end())
                    {
                        PositionIndex posIndex = getPositionIndex(*it2);

                        if (i < posIndex.x)
                        {
                            // 移動先の位置をセットする
                            setNewPosition2(*it2, posIndex);
                        }

                        it2++;
                    }

                    it1++;
                }
            }
        }
    }
}


void GameScene::movingBlocksAnimation2()
{
    // コマの新しい位置をセット
    searchNewPosition2();

    //　新しい位置がセットされたコマのアニメーション実行
    moveBlock();

    // アニメーション終了時に次の処理を開始する
    scheduleOnce(schedule_selector(GameScene::movedBlocks), MOVING_TIME);

    return;
}


void GameScene::showLabel()
{
    // 背景画像サイズ取得
    CCSize bgSize = m_background->getContentSize();

    // 残コマ数表示
    int tagsForLabel[] = {kTagRedLabel, kTagBlueLabel, kTagYellowLabel, kTagGreenLabel, kTagGrayLabel};
    const char* fontNames[] = {FONT_RED, FONT_BLUE, FONT_YELLOW, FONT_GREEN, FONT_GRAY};
    float heightRate[] = {0.61, 0.51, 0.41, 0.31, 0.21};


    // コマ種類分ループ
    vector<kBlock>::iterator it = blockTypes.begin();
    while (it != blockTypes.end())
    {
        // コマ残数表示
        int count = m_blockTags[*it].size();
        const char* countStr = ccsf("%02d", count);

        CCLabelBMFont* label = (CCLabelBMFont*)m_background->getChildByTag(tagsForLabel[*it]);
        if (!label)
        {
            // コマ残数ラベル生成
            label = CCLabelBMFont::create(countStr, fontNames[*it]);
            label->setPosition(ccp(bgSize.width * 0.78, bgSize.height * heightRate[*it]));

            m_background->addChild(label, kZOrderLabel, tagsForLabel[*it]);
        }
        else
        {
            label->setString(countStr);
        }

        it++;
    }


    // スコア表示
    const char* scoreStr = ccsf("%d", m_score);
    CCLabelBMFont* scoreLabel = (CCLabelBMFont*)m_background->getChildByTag(kTagScoreLabel);
    if (!scoreLabel)
    {
        // スコアラベル生成
        scoreLabel = CCLabelBMFont::create(scoreStr, FONT_WHITE);
        scoreLabel->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.75));
        m_background->addChild(scoreLabel, kZOrderLabel, kTagScoreLabel);
    }
    else
    {
        scoreLabel->setString(scoreStr);
    }

}


bool GameScene::existsSameBlock()
{
    // コマの種類分ループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end())
    {
        // 各種類のコマ数分ループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end())
        {
            if (getSameColorBlockTags(*it2, *it1).size() > 1)
            {
                // 隣り合うコマがある場合
                return true;
            }

            it2++;
        }

        it1++;
    }

    return false;
}


/**
 * ハイスコアラベル表示
 */
void GameScene::showHighScoreLabel()
{
    CCSize bgSize = m_background->getContentSize();

    // ハイスコア表示
    int highScore = CCUserDefault::sharedUserDefault()->getIntegerForKey(KEY_HIGHSCORE, 0);
    const char* highScoreStr = ccsf("%d", highScore);
    CCLabelBMFont* highScoreLabel = (CCLabelBMFont*)m_background->getChildByTag(kTagHighScoreLabel);
    if (!highScoreLabel)
    {
        // ハイスコアラベル生成
        highScoreLabel = CCLabelBMFont::create(highScoreStr, FONT_WHITE);
        highScoreLabel->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.87));

        m_background->addChild(highScoreLabel, kZOrderLabel, kTagHighScoreLabel);
    }
    else
    {
        highScoreLabel->setString(highScoreStr);
    }
}



void GameScene::saveHighScore()
{
    // ハイスコアを保持する
    CCUserDefault* userDefault = CCUserDefault::sharedUserDefault();

    // ハイスコアを取得する
    int oldHighScore = userDefault->getIntegerForKey(KEY_HIGHSCORE, 0);
    if (oldHighScore < m_score)
    {
        userDefault->setIntegerForKey(KEY_HIGHSCORE, m_score);
        userDefault->flush();

        // ハイスコアを表示
        showHighScoreLabel();
    }
}


void GameScene::menuResetCallback(cocos2d::CCObject* pSender)
{
    GameScene* scene = GameScene::create();
    CCDirector::sharedDirector()->replaceScene((CCScene*)scene);
}



void GameScene::showResetButton()
{

    CCSize bgSize = m_background->getContentSize();


    // リセットボタン作成
    CCMenuItemImage* resetButton = CCMenuItemImage::create(PNG_RESET, PNG_RESET, this, menu_selector(GameScene::menuResetCallback));
    resetButton->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.1));

    // メニュー作成
    CCMenu* menu = CCMenu::create(resetButton, NULL);
    menu->setPosition(CCPointZero);
    m_background->addChild(menu);
}


void GameScene::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
}


void GameScene::keyMenuClicked()
{
    menuResetCallback(NULL);
}
