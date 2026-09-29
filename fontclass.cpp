#include "fontclass.h"

FontClass::FontClass()
{
    m_Font = 0;
    m_Texture = 0;
}

FontClass::FontClass(const FontClass&)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int fontChoice)
{
    char fontFileName[128];
    char fontTextureFileName[128];
    bool result;
    
    // 사용 가능한 폰트 중 하나를 고르고, 아니면 기본으로 첫 번째 폰트를 쓴다.
    switch (fontChoice)
    {
    case 0:
        {
            strcpy_s(fontFileName, "data/font/font01.txt");
            strcpy_s(fontTextureFileName, "data/font/font01.tga");
            m_fontHeight = 32.0f;
            m_spaceSize = 3;
            break;
        }
    default:
        {
            strcpy_s(fontFileName, "data/font/font01.txt");
            strcpy_s(fontTextureFileName, "data/font/font01.tga");
            m_fontHeight = 32.0f;
            m_spaceSize = 3;
            break;
        }
    }
    // 폰트 데이터가 담긴 텍스트 파일을 불러온다.
    result = LoadFontData(fontFileName);
    if(!result)
    {
        return false;
    }

    // 폰트 문자들이 담긴 텍스처를 불러온다.
    // 폰트 문자들이 담긴 텍스처를 불러온다.
    result = LoadTexture(device, deviceContext, fontTextureFileName);
    if(!result)
    {
        return false;
    }

    return true;
}

void FontClass::Shutdown()
{
    // 폰트 텍스처를 해제한다.
    ReleaseTexture();

    // 폰트 데이터를 해제한다.
    ReleaseFontData();

    return;
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
    VertexType* vertexPtr;
    int numLetters, index, i, letter;


    // 입력 정점을 VertexType 구조체로 캐스팅한다.
    vertexPtr = (VertexType*)vertices;

    // 문장의 글자 수를 구한다.
    numLetters = (int)strlen(sentence);

    // 정점 배열의 인덱스를 초기화한다.
    index = 0;
    // 각 글자를 사각형에 그린다.
    for(i=0; i<numLetters; i++)
    {
        letter = ((int)sentence[i]) - 32;

        // 글자가 공백이면 3픽셀만 이동한다.
        if(letter == 0)
        {
            drawX = drawX + m_spaceSize;
        }
        else
        {
            // 사각형의 첫 번째 삼각형.
            vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // 좌상단.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
            index++;

            vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.0f);  // 우하단.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
            index++;

            vertexPtr[index].position = XMFLOAT3(drawX, (drawY - m_fontHeight), 0.0f);  // 좌하단.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
            index++;

            // 사각형의 두 번째 삼각형.
            vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // 좌상단.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
            index++;

            vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // 우상단.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
            index++;

            vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - m_fontHeight), 0.0f);  // 우하단.
            vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
            index++;

            // 글자 폭에 1픽셀을 더한 만큼 그리기 x 위치를 갱신한다.
            drawX = drawX + m_Font[letter].size + 1.0f;
        }
    }

    return;
}

int FontClass::GetSentencePixelLength(char* sentence)
{
    int pixelLength, numLetters, i, letter;
    
    pixelLength = 0;
    numLetters = (int)strlen(sentence);

    for(i=0; i<numLetters; i++)
    {
        letter = ((int)sentence[i]) - 32;

        // 글자가 공백이면 3픽셀로 계산한다.
        if(letter == 0)
        {
            pixelLength += m_spaceSize;
        }
        else
        {
            pixelLength += (m_Font[letter].size + 1);
        }
    }

    return pixelLength;
}

int FontClass::GetFontHeight()
{
    return (int)m_fontHeight;
}

bool FontClass::LoadFontData(char* fileName)
{
    ifstream fin;
    int i;
    char temp;
    
    // 폰트 간격 버퍼를 생성한다.
    m_Font = new FontType[95];
    
    // 폰트 크기와 문자 간격을 읽어 들인다.
    fin.open(fileName);
    if(fin.fail())
    {
        return false;
    }

    // 텍스트에 쓰이는 95개 아스키 문자를 읽어 들인다.
    for(i=0; i<95; i++)
    {
        fin.get(temp);
        while(temp != ' ')
        {
            fin.get(temp);
        }
        fin.get(temp);
        while(temp != ' ')
        {
            fin.get(temp);
        }

        fin >> m_Font[i].left;
        fin >> m_Font[i].right;
        fin >> m_Font[i].size;
    }

    // 파일을 닫는다.
    fin.close();

    return true;
}

void FontClass::ReleaseFontData()
{
    // 폰트 데이터 배열을 해제한다.
    if(m_Font)
    {
        delete [] m_Font;
        m_Font = 0;
    }

    return;
}

bool FontClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fileName)
{
    bool result;


    // 폰트 텍스처 객체를 생성하고 초기화한다.
    m_Texture = new TextureClass;

    result = m_Texture->Initialize(device, deviceContext, fileName);
    if(!result)
    {
        return false;
    }

    return true;
}

void FontClass::ReleaseTexture()
{
    // 텍스처 객체를 해제한다.
    if(m_Texture)
    {
        m_Texture->Shutdown();
        delete m_Texture;
        m_Texture = 0;
    }

    return;
}
