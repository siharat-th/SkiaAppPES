#ifndef __DECBWSUI_LOADER_H__
#define __DECBWSUI_LOADER_H__

#include "DecBWSInfoBox.h"
#include "DeclarativeUI.h"

class DecBWSUI : public DecContainer {
public:
    DecBWSUI();
    virtual ~DecBWSUI() override;

    virtual void update() override;
    virtual void init() override;

    int nWorkingMode;
    int nRedMode;
    int nGreenMode;

    // RED parameters
    //float fLeverSpeed;

    float fLeftWeight;
    float fRightWeight;

    int nTrainingMode;

    float fMaxSpeed;

    // Green parameter
    int nPosMode;

    float fLeftPos;
    float fRightPos;

    bool isTraining;

    // Yello parameter
    int nYelloMode;

    int nLeverSpeedMode;
    int nFallingLimitMode;
    int nPIDMode;

    float fLeftLeverSpeed;
    float fRightLeverSpeed;

    float fLeftFallingLimit;
    float fRightFallingLimit;

    float fPID[20];

    DecButton* _pButtonStartTraining;
    DecButton* _pButtonStopTraining;

    DecPageContainer* _pWorkingPages;
    DecWidget* _pLeverMode;
    DecWidget* _pWeightSupportMode;
    DecWidget* _pTrainingMode;

    DecWidget* _pTurningMode;
    DecWidget* _pTrainingMode2;
    DecWidget* _pStatusMode;

    DecBWSInfoBox* _pTextModeName;

    DecWidget* _pLeverSpeedMode;
    DecWidget* _pFallingLimitMode;
    DecWidget* _pPIDMode;

    

    bool bJoyUp;
    bool bJoyLeft;
    bool bJoyRight;
    bool bJoyDown;

    bool bDUp;
    bool bDLeft;
    bool bDRight;
    bool bDDown;

    bool bA;
    bool bB;

    bool bR2;

    float jX;
    float jY;

    bool bLastDUp;
    bool bLastDDown;

    DecBWSInfoBox* pInfoLeverSpeed;

    DecWidget* pArrowLeverUp;
    DecWidget* pArrowLeverDown;

    bool bLeftWeight;
    bool bRightWeight;

    DecWidget* pArrowWeightSupportUp;
    DecWidget* pArrowWeightSupportDown;

    DecBWSInfoBox* pLeftWeightSupportButton;
    DecBWSInfoBox* pRightWeightSupportButton;

    DecText* pLeftWeightSupportText;
    DecText* pRightWeightSupportText;

    DecText* pLeftWeightSupportText2;
    DecText* pRightWeightSupportText2;

    DecBWSInfoBox* pTrainingModeStraight;
    DecBWSInfoBox* pTrainingModeFollow;
    DecBWSInfoBox* pTrainingModeManual;

    DecBWSInfoBox* pTrainingMaxSpeedBox;
    DecBWSInfoBox* pTrainingMaxSpeedBox2;

    void setTrainingMode(int m);

    DecWidget* pArrowMaxSpeedUp;
    DecWidget* pArrowMaxSpeedDown;

    DecWidget* pArrowLeftPosUp;
    DecWidget* pArrowLeftPosDown;
    DecWidget* pArrowRightPosUp;
    DecWidget* pArrowRightPosDown;

    DecBWSInfoBox* pInfoLeftPos;
    DecBWSInfoBox* pInfoRightPos;

    DecBWSInfoBox* pInfoLeftLeverSpeed;
    DecBWSInfoBox* pInfoRightLeverSpeed;

    DecBWSInfoBox* pInfoLeftFallingLimit;
    DecBWSInfoBox* pInfoRightFallingLimit;

    DecBWSInfoBox* pInfoLeftWheelP;
    DecBWSInfoBox* pInfoLeftWheelI;
    DecBWSInfoBox* pInfoLeftWheelD;

    DecBWSInfoBox* pInfoRightWheelP;
    DecBWSInfoBox* pInfoRightWheelI;
    DecBWSInfoBox* pInfoRightWheelD;

    DecBWSInfoBox* pInfoLeftBWSP;
    DecBWSInfoBox* pInfoLeftBWSI;
    DecBWSInfoBox* pInfoLeftBWSD;

    DecBWSInfoBox* pInfoRightBWSP;
    DecBWSInfoBox* pInfoRightBWSI;
    DecBWSInfoBox* pInfoRightBWSD;

    DecBWSInfoBox* pInfoLeftRefA;
    DecBWSInfoBox* pInfoLeftRefB;
    DecBWSInfoBox* pInfoLeftRefC;
    DecBWSInfoBox* pInfoLeftRefD;

    DecBWSInfoBox* pInfoRightRefA;
    DecBWSInfoBox* pInfoRightRefB;
    DecBWSInfoBox* pInfoRightRefC;
    DecBWSInfoBox* pInfoRightRefD;

    DecColumnContainer *pKeyPad;

    std::string KeyPadText;

    void ParamClear();
    void YellowSetUIValues();
    void KeyPadAdd( const char *ch );
    void KeyPadSetValue();
    void YellowSetKeyPadString();
    void YellowGetKeyPadString();

    void homeBWS();
    void setBWSMode( int m, float d1=0, float d2=0, float d3=0, float d4=0 );
    void setBWSLeverSpeed( float s );

    int updateCount;

    virtual void js_callback( std::string message ) override;

    unsigned int parseByte( std::string str, int index );
    int parseInt( std::string str, int index );
    float parseFloat( std::string str, int index );

    unsigned int hex2int( const char c );
};

DecBWSUI& BWSUI();

#endif  //__DECBWSUI_LOADER_H__