#include "Function.h"

#include "../math/Vector4.h"
#include "../math/Vector3.h"
#include "../math/Vector2.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <cassert>

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
    // 1. 中で必要となる変数の宣言
    // 2. ファイルを開く
    // 3. 実際にファイルを読み、ModelDataを構築していく
    // 4. ModelDataを返す

    /// 1.2.必要な変数の宣言とファイルを開く

    ModelData modelData; //構築するModelData
    std::vector<Vector4> positions; //位置
    std::vector<Vector3> normals; //法線
    std::vector<Vector2> texcoords; //テクスチャ座標
    std::string line; //ファイルから読んだ1行を格納するもの

    std::ifstream file(directoryPath + "/" + filename); //ファイルを開く
    assert(file.is_open()); //とりあえず開けなかったら止める

    ///3.ファイルを読み、ModelDataを構築
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier; //先頭の識別子を読む


        //identifierに応じた処理

        ///頂点情報を読む
        if (identifier == "v") {
            Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.w = 1.0f;
            positions.push_back(position);
        } 
        else if (identifier == "vt") {
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y;
            texcoords.push_back(texcoord);
        } 
        else if (identifier == "vn") {
            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }

        ///三角形を作る

        else if (identifier == "f") {
            VertexData triangle[3];
            //面は三角形限定。その他は未対応
            for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                std::string vertexDefinition;
                s >> vertexDefinition;
                //頂点の要素のIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
                std::istringstream v(vertexDefinition);
                uint32_t elementIndices[3];
                for (int32_t element = 0; element < 3; ++element) {
                    std::string index;
                    std::getline(v, index, '/'); //区切りでインデックスを読んでいく
                    elementIndices[element] = std::stoi(index);
                }
                //要素へのIndexから、実際の要素の値を取得して、頂点を構築する
                Vector4 position = positions[elementIndices[0] - 1];
                Vector2 texcoord = texcoords[elementIndices[1] - 1];
                Vector3 normal = normals[elementIndices[2] - 1];
                //VertexData vertex = { position,texcoord,normal };
                //modelData.vertices.push_back(vertex);

                ///右手系から左手系へ

                position.x *= -1.0f;
                normal.x *= -1.0f;

                ///Texture座標の原点

                texcoord.y = 1.0f - texcoord.y;

                ///右手系から左手系へ

                triangle[faceVertex] = { position,texcoord,normal };

            }

            //頂点を逆順で登録することで、回り順を逆にする
            modelData.vertices.push_back(triangle[2]);
            modelData.vertices.push_back(triangle[1]);
            modelData.vertices.push_back(triangle[0]);
        }

        ///obj読み込みにmaterial読み込みを追加

        else if (identifier == "mtllib") {
            //materialTempalateLibraryファイルの名前を取得する
            std::string materialFilename;
            s >> materialFilename;
            //基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイルを渡す
            modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
        }
    }

    return modelData;
}


