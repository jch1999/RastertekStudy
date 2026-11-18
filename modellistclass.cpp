////////////////////////////////////////////////////////////////////////////////
// Filename: modellistclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modellistclass.h"

ModelListClass::ModelListClass()
{
    m_ModelInfoList = 0;
}

ModelListClass::ModelListClass(const ModelListClass& other)
{
}

ModelListClass::~ModelListClass()
{
}

void ModelListClass::Initialize(int numModels)
{
    int i;
    
    // 모델 개수를 저장한다.
    m_modelCount = numModels;
    
    // 모델 정보의 목록 배열을 생성한다.
    m_ModelInfoList = new ModelInfoType[m_modelCount];
    // 현재 시간으로 난수 생성기를 시도한다.
    srand(unsigned int(time(NULL)));
    
    // 모든 모델을 순회하며 위치를 무작위로 생성한다.
    for (i = 0; i < m_modelCount; i++)
    {
        // 관찰자 앞쪼겡 모델의 무작위 위치를 생성한다.
        m_ModelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
        m_ModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
        m_ModelInfoList[i].positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f + 5.0f;
    }
    
    return;
}

void ModelListClass::Shutdown()
{
    // 모델 정보 목록을 해제한다.
    if (m_ModelInfoList)
    {
        delete[] m_ModelInfoList;
        m_ModelInfoList = 0;
    }
    
    return;
}

int ModelListClass::GetModelCount()
{
    return m_modelCount;
}

void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ)
{
    positionX = m_ModelInfoList[index].positionX;
    positionY = m_ModelInfoList[index].positionY;
    positionZ = m_ModelInfoList[index].positionZ;
}
