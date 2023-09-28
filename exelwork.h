#ifndef EXELWORK_H
#define EXELWORK_H


#include <cmath>
#include <format>
#include <regex>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>


class ExelFile
{
public:

    class SheetExel{
    public:

        static const std::string uint_to_str(uint num){
            std::string answer="";
            if (num==0) return "0";
            while(num){
                answer=char(num%10+'0')+answer;
                num/=10;
            }
            return answer;
        }

        static inline const std::string convert_Human_Xpos_to_Exel_Xpos(uint xpos){

            //if (!xpos) return "A";
            std::string answer="";
            while(xpos){
                --xpos;
                answer=char(xpos%('Z'-'A'+1)+'A')+answer;
                xpos/=('Z'-'A'+1);
            }
            return answer;
        }

        class ExelCeil{
            static const inline bool isNumeric(const std::string& val){
                if (val.length()==0 || !(val[0]>='0' && val[0]<='9' || val[0]=='-'))
                    return false;
                int count_point=1;
                for (int i=1; i<val.length(); i++){
                    if (val[i]=='.') if(--count_point<0) return false;
                    if (val[i]<'0' || val[i]>'9') return false;
                }
                return true;
            }

            static const inline bool isHyperlink(const std::string& val){
                static const std::regex URL("(?:https?|ftp|file)://[-a-zA-Z0-9+&@#/%?=~_|!:, .;]*[-a-zA-Z0-9+&@#/%=~_|]");
                return std::regex_match(val, URL);
            }


        public:

            std::string _t, _s, _v, _r;

            ExelCeil(){
            }





            void operator=(const std::string value){
                //std::cout << "Sperma";
                content=value;
                if (value!="" && isNumeric(value)){
                    _t="n";
                    _s="0";
                    return;
                }
                if(value!="" && isHyperlink(value)){
                    _t="s";
                    _s="1";
                    return;
                }
                _t="s";
                _s="0";
            }

            const std::string val(){return content;}

        private:

            std::string content;
        };




        SheetExel(std::string name, unsigned int sheetId /*, unsigned int r_id*/){
            this->name=name;
            this->sheetId=sheetId;
            //this->r_id=r_id;
        }

        ~SheetExel(){
            for (int i=0; i<this->GridSize.second; i++){
                for (int j=0; j<GridSize.first; j++)
                    delete GridCeil[i][j];
            }
        }

        inline void print_4_workbool_xml(std::ofstream& fout, unsigned int r_id){
            fout << std::format("<sheet state=\"visible\" name=\"{}\" sheetId=\"{}\" r:id=\"rId{}\"/>", name, sheetId, r_id);
        }



        ExelCeil* operator[](std::pair<uint, uint> pos){ //{x, y}
            //std::cout << "Zalupa";
            if (GridSize.first<pos.first){
                uint delta=pos.first-GridSize.first;
                std::vector<ExelCeil*> filler(delta, nullptr);
                for (uint i=0; i<GridCeil.size(); i++){
                    GridCeil[i].insert(GridCeil[i].end(), filler.begin(), filler.end());
                }
            }
            GridSize.first=std::max(pos.first, GridSize.first);
            for (uint i=GridSize.second; i<pos.second; i++){
                GridCeil.push_back(std::vector<ExelCeil*>(GridSize.first, nullptr));
            }
            GridSize.second=GridCeil.size();

            //std::cout << "Chlen";

            if(GridCeil[pos.second-1][pos.first-1]==nullptr){
                GridCeil[pos.second-1][pos.first-1] = new ExelCeil();
            }
            //std::cout << "Hui";
            return GridCeil[pos.second-1][pos.first-1];
        }

        const std::pair<uint, uint> size() {return GridSize;}
        const uint heigth() {return GridSize.second;}
        const uint width() {return GridSize.first;}


    private:
        std::string name;
        unsigned int sheetId;
        //unsigned int r_id;
        friend ExelFile;
    protected:

        std::pair<uint, uint> GridSize={0,0};
        std::vector<std::vector<ExelCeil*>> GridCeil;
    };


    ExelFile(){}

    ~ExelFile(){

        for (auto now: Sheets){
            delete now;
        }
    }

    //  <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    //  <Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships"><Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme" Target="theme/theme1.xml"/><Relationship Id="rId2" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" Target="styles.xml"/><Relationship Id="rId3" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings" Target="sharedStrings.xml"/>

    // "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\"><Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme\" Target=\"theme/theme1.xml\"/><Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/><Relationship Id=\"rId3\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings\" Target=\"sharedStrings.xml\"/>"

    void make_XLXS(std::string FileName){
        if (!std::filesystem::is_directory(FileName)) std::filesystem::create_directory(FileName);
        if (!std::filesystem::is_directory(FileName+"/_rels")) std::filesystem::create_directory(FileName+"/_rels");
        if (!std::filesystem::is_directory(FileName+"/xl")) std::filesystem::create_directory(FileName+"/xl");

        std::ofstream file_printer;
        char path[100];

        file_printer.open(FileName+"/[Content_Types].xml");
        file_printer << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\"><Default ContentType=\"application/xml\" Extension=\"xml\"/><Default ContentType=\"application/vnd.openxmlformats-package.relationships+xml\" Extension=\"rels\"/><Override ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml\" PartName=\"/xl/styles.xml\"/><Override ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml\" PartName=\"/xl/sharedStrings.xml\"/><Override ContentType=\"application/vnd.openxmlformats-officedocument.theme+xml\" PartName=\"/xl/theme/theme1.xml\"/><Override ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml\" PartName=\"/xl/workbook.xml\"/>";
        for (uint i=1; i<=Sheet_count; i++){
            file_printer << std::format("<Override ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\" PartName=\"/xl/worksheets/sheet{0}.xml\"/><Override ContentType=\"application/vnd.openxmlformats-officedocument.drawing+xml\" PartName=\"/xl/drawings/drawing{0}.xml\"/>", i);
        }
        file_printer << "</Types>";
        file_printer.close();

        file_printer.open(FileName+"/_rels/.rels");
        file_printer << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\"><Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"xl/workbook.xml\"/></Relationships>";
        file_printer.close();

        FileName+="/xl";
        file_printer.open(FileName+"/workbook.xml");
        Workbook_print(file_printer);
        file_printer.close();

        if (!std::filesystem::is_directory(FileName+"/drawings")) std::filesystem::create_directory(FileName+"/drawings");
        for (int i=1; i<=Sheet_count; i++){
            file_printer.open(std::format("{}/drawings/drawing{}.xml", FileName, i));
            file_printer << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<xdr:wsDr xmlns:xdr=\"http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing\" xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:c=\"http://schemas.openxmlformats.org/drawingml/2006/chart\" xmlns:cx=\"http://schemas.microsoft.com/office/drawing/2014/chartex\" xmlns:cx1=\"http://schemas.microsoft.com/office/drawing/2015/9/8/chartex\" xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" xmlns:dgm=\"http://schemas.openxmlformats.org/drawingml/2006/diagram\" xmlns:x3Unk=\"http://schemas.microsoft.com/office/drawing/2010/slicer\" xmlns:sle15=\"http://schemas.microsoft.com/office/drawing/2012/slicer\"/>";
            file_printer.close();
        }

        if (!std::filesystem::is_directory(FileName+"/theme")) std::filesystem::create_directory(FileName+"/theme");
        file_printer.open(std::format("{}/theme/theme1.xml", FileName));
        file_printer << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<a:theme xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" name=\"Sheets\"><a:themeElements><a:clrScheme name=\"Sheets\"><a:dk1><a:srgbClr val=\"000000\"/></a:dk1><a:lt1><a:srgbClr val=\"FFFFFF\"/></a:lt1><a:dk2><a:srgbClr val=\"000000\"/></a:dk2><a:lt2><a:srgbClr val=\"FFFFFF\"/></a:lt2><a:accent1><a:srgbClr val=\"4285F4\"/></a:accent1><a:accent2><a:srgbClr val=\"EA4335\"/></a:accent2><a:accent3><a:srgbClr val=\"FBBC04\"/></a:accent3><a:accent4><a:srgbClr val=\"34A853\"/></a:accent4><a:accent5><a:srgbClr val=\"FF6D01\"/></a:accent5><a:accent6><a:srgbClr val=\"46BDC6\"/></a:accent6><a:hlink><a:srgbClr val=\"1155CC\"/></a:hlink><a:folHlink><a:srgbClr val=\"1155CC\"/></a:folHlink></a:clrScheme><a:fontScheme name=\"Sheets\"><a:majorFont><a:latin typeface=\"Arial\"/><a:ea typeface=\"Arial\"/><a:cs typeface=\"Arial\"/></a:majorFont><a:minorFont><a:latin typeface=\"Arial\"/><a:ea typeface=\"Arial\"/><a:cs typeface=\"Arial\"/></a:minorFont></a:fontScheme><a:fmtScheme name=\"Office\"><a:fillStyleLst><a:solidFill><a:schemeClr val=\"phClr\"/></a:solidFill><a:gradFill rotWithShape=\"1\"><a:gsLst><a:gs pos=\"0\"><a:schemeClr val=\"phClr\"><a:lumMod val=\"110000\"/><a:satMod val=\"105000\"/><a:tint val=\"67000\"/></a:schemeClr></a:gs><a:gs pos=\"50000\"><a:schemeClr val=\"phClr\"><a:lumMod val=\"105000\"/><a:satMod val=\"103000\"/><a:tint val=\"73000\"/></a:schemeClr></a:gs><a:gs pos=\"100000\"><a:schemeClr val=\"phClr\"><a:lumMod val=\"105000\"/><a:satMod val=\"109000\"/><a:tint val=\"81000\"/></a:schemeClr></a:gs></a:gsLst><a:lin ang=\"5400000\" scaled=\"0\"/></a:gradFill><a:gradFill rotWithShape=\"1\"><a:gsLst><a:gs pos=\"0\"><a:schemeClr val=\"phClr\"><a:satMod val=\"103000\"/><a:lumMod val=\"102000\"/><a:tint val=\"94000\"/></a:schemeClr></a:gs><a:gs pos=\"50000\"><a:schemeClr val=\"phClr\"><a:satMod val=\"110000\"/><a:lumMod val=\"100000\"/><a:shade val=\"100000\"/></a:schemeClr></a:gs><a:gs pos=\"100000\"><a:schemeClr val=\"phClr\"><a:lumMod val=\"99000\"/><a:satMod val=\"120000\"/><a:shade val=\"78000\"/></a:schemeClr></a:gs></a:gsLst><a:lin ang=\"5400000\" scaled=\"0\"/></a:gradFill></a:fillStyleLst><a:lnStyleLst><a:ln w=\"6350\" cap=\"flat\" cmpd=\"sng\" algn=\"ctr\"><a:solidFill><a:schemeClr val=\"phClr\"/></a:solidFill><a:prstDash val=\"solid\"/><a:miter lim=\"800000\"/></a:ln><a:ln w=\"12700\" cap=\"flat\" cmpd=\"sng\" algn=\"ctr\"><a:solidFill><a:schemeClr val=\"phClr\"/></a:solidFill><a:prstDash val=\"solid\"/><a:miter lim=\"800000\"/></a:ln><a:ln w=\"19050\" cap=\"flat\" cmpd=\"sng\" algn=\"ctr\"><a:solidFill><a:schemeClr val=\"phClr\"/></a:solidFill><a:prstDash val=\"solid\"/><a:miter lim=\"800000\"/></a:ln></a:lnStyleLst><a:effectStyleLst><a:effectStyle><a:effectLst/></a:effectStyle><a:effectStyle><a:effectLst/></a:effectStyle><a:effectStyle><a:effectLst><a:outerShdw blurRad=\"57150\" dist=\"19050\" dir=\"5400000\" algn=\"ctr\" rotWithShape=\"0\"><a:srgbClr val=\"000000\"><a:alpha val=\"63000\"/></a:srgbClr></a:outerShdw></a:effectLst></a:effectStyle></a:effectStyleLst><a:bgFillStyleLst><a:solidFill><a:schemeClr val=\"phClr\"/></a:solidFill><a:solidFill><a:schemeClr val=\"phClr\"><a:tint val=\"95000\"/><a:satMod val=\"170000\"/></a:schemeClr></a:solidFill><a:gradFill rotWithShape=\"1\"><a:gsLst><a:gs pos=\"0\"><a:schemeClr val=\"phClr\"><a:tint val=\"93000\"/><a:satMod val=\"150000\"/><a:shade val=\"98000\"/><a:lumMod val=\"102000\"/></a:schemeClr></a:gs><a:gs pos=\"50000\"><a:schemeClr val=\"phClr\"><a:tint val=\"98000\"/><a:satMod val=\"130000\"/><a:shade val=\"90000\"/><a:lumMod val=\"103000\"/></a:schemeClr></a:gs><a:gs pos=\"100000\"><a:schemeClr val=\"phClr\"><a:shade val=\"63000\"/><a:satMod val=\"120000\"/></a:schemeClr></a:gs></a:gsLst><a:lin ang=\"5400000\" scaled=\"0\"/></a:gradFill></a:bgFillStyleLst></a:fmtScheme></a:themeElements></a:theme>";
        file_printer.close();

        if (!std::filesystem::is_directory(FileName+"/_rels")) std::filesystem::create_directory(FileName+"/_rels");
        file_printer.open(std::format("{}/_rels/workbook.xml.rels", FileName));
        file_printer << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\"><Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme\" Target=\"theme/theme1.xml\"/><Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/><Relationship Id=\"rId3\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings\" Target=\"sharedStrings.xml\"/>";
        for (int i=1; i<=Sheet_count; i++)
            file_printer << std::format("<Relationship Id=\"rId{}\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\" Target=\"worksheets/sheet{}.xml\"/>", i+3, i);
        file_printer << "</Relationships>";
        file_printer.close();

        if (!std::filesystem::is_directory(FileName+"/worksheets")) std::filesystem::create_directory(FileName+"/worksheets");
        if (!std::filesystem::is_directory(FileName+"/worksheets/_rels")) std::filesystem::create_directory(FileName+"/worksheets/_rels");
        uint couner_v=0;
        std::vector <std::string> strings;
        for (int sheet_ind=0; sheet_ind<Sheet_count; sheet_ind++){
            std::vector<std::string> links;
            std::vector<std::string> links_rels;
            file_printer.open(std::format("{}/worksheets/sheet{}.xml", FileName, sheet_ind+1));
            file_printer << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:mx=\"http://schemas.microsoft.com/office/mac/excel/2008/main\" xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" xmlns:mv=\"urn:schemas-microsoft-com:mac:vml\" xmlns:x14=\"http://schemas.microsoft.com/office/spreadsheetml/2009/9/main\" xmlns:x15=\"http://schemas.microsoft.com/office/spreadsheetml/2010/11/main\" xmlns:x14ac=\"http://schemas.microsoft.com/office/spreadsheetml/2009/9/ac\" xmlns:xm=\"http://schemas.microsoft.com/office/excel/2006/main\"><sheetPr><outlinePr summaryBelow=\"0\" summaryRight=\"0\"/></sheetPr><sheetViews><sheetView workbookViewId=\"0\"/></sheetViews><sheetFormatPr customHeight=\"1\" defaultColWidth=\"12.63\" defaultRowHeight=\"15.75\"/>";
            //sheetData
            file_printer << "<sheetData>";
            for (int row_ind=1; row_ind<=Sheets[sheet_ind]->heigth(); row_ind++){
                file_printer << std::format("<row r=\"{}\">", row_ind);
                //ceils in row info
                for (int column_ind=1; column_ind<=Sheets[sheet_ind]->width(); column_ind++){
                    if (Sheets[sheet_ind]->GridCeil[row_ind-1][column_ind-1]!=nullptr){
                        //std::cout << std::format("{}, {}\n", column_ind, row_ind);
                        SheetExel::ExelCeil* currentCeil=((*Sheets[sheet_ind])[std::pair{column_ind, row_ind}]);
                        file_printer << std::format("<c r=\"{}{}\" s=\"{}\"{}><v>{}</v></c>",
                                                    SheetExel::convert_Human_Xpos_to_Exel_Xpos(column_ind), row_ind,
                                                    currentCeil->_s, currentCeil->_t=="s" ? " t=\""+currentCeil->_t+"\"" : "",
                                                    currentCeil->_t=="s" ? SheetExel::uint_to_str(couner_v++) : currentCeil->val()
                                                    );
                        if (currentCeil->_s=="1"){
                            links.push_back(std::format("<hyperlink r:id=\"rId{}\" ref=\"{}{}\"/>",
                                                        links.size()+1, SheetExel::convert_Human_Xpos_to_Exel_Xpos(column_ind), row_ind)
                                            );
                            links_rels.push_back(std::format("<Relationship Id=\"rId{}\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/hyperlink\" Target=\"{}\" TargetMode=\"External\"/>",
                                                             links.size(), currentCeil->val()
                                                             ));
                        }
                        if (currentCeil->_t=="s") strings.push_back(std::format("<si><t>{}</t></si>", currentCeil->val()));
                    }
                    /*else{
                        file_printer << std::format("<c r=\"{}{}\" s=\"1\" t=\"s\"><v>{}</v></c>",
                            SheetExel::convert_Human_Xpos_to_Exel_Xpos(column_ind), row_ind,
                            SheetExel::uint_to_str(couner_v++)
                        );
                        strings.push_back(std::format("<si><t>{}</t></si>", ""));
                    }*/
                }
                file_printer << "</row>";
            }
            //for (auto now: strings) std::cout << now << "\n";
            file_printer << "</sheetData>";

            //hyperlinks
            if (links.size()){
                file_printer << "<hyperlinks>";
                for (auto now: links) file_printer << now;
                file_printer << "</hyperlinks>";
            }
            std::ofstream rels_worksheet_printer(std::format("{}/worksheets/_rels/sheet{}.xml.rels", FileName, sheet_ind+1));
            rels_worksheet_printer << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">";
            for (auto now: links_rels) rels_worksheet_printer << now;
            rels_worksheet_printer << "<Relationship Id=\"rId" << links.size()+1 << "\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/drawing\" Target=\"../drawings/drawing1.xml\"/></Relationships>";



            file_printer << std::format("<drawing r:id=\"rId{}\"/></worksheet>", links.size()+1);
            file_printer.close();
        }

        file_printer.open(std::format("{}/sharedStrings.xml", FileName));
        file_printer << std::format("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<sst xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" count=\"{0}\" uniqueCount=\"{0}\">", strings.size());
        for (auto now: strings) file_printer << now;
        file_printer << "</sst>";
        file_printer.close();

        file_printer.open(std::format("{}/styles.xml", FileName));
        file_printer << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<styleSheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:x14ac=\"http://schemas.microsoft.com/office/spreadsheetml/2009/9/ac\" xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\"><fonts count=\"3\"><font><sz val=\"10.0\"/><color rgb=\"FF000000\"/><name val=\"Arial\"/><scheme val=\"minor\"/></font><font><u/><color rgb=\"FF0000FF\"/></font><font><color theme=\"1\"/><name val=\"Arial\"/><scheme val=\"minor\"/></font></fonts><fills count=\"2\"><fill><patternFill patternType=\"none\"/></fill><fill><patternFill patternType=\"lightGray\"/></fill></fills><borders count=\"1\"><border/></borders><cellStyleXfs count=\"1\"><xf borderId=\"0\" fillId=\"0\" fontId=\"0\" numFmtId=\"0\" applyAlignment=\"1\" applyFont=\"1\"/></cellStyleXfs><cellXfs count=\"3\"><xf borderId=\"0\" fillId=\"0\" fontId=\"0\" numFmtId=\"0\" xfId=\"0\" applyAlignment=\"1\" applyFont=\"1\"><alignment readingOrder=\"0\" shrinkToFit=\"0\" vertical=\"bottom\" wrapText=\"0\"/></xf><xf borderId=\"0\" fillId=\"0\" fontId=\"1\" numFmtId=\"0\" xfId=\"0\" applyAlignment=\"1\" applyFont=\"1\"><alignment readingOrder=\"0\"/></xf><xf borderId=\"0\" fillId=\"0\" fontId=\"2\" numFmtId=\"0\" xfId=\"0\" applyAlignment=\"1\" applyFont=\"1\"><alignment readingOrder=\"0\"/></xf></cellXfs><cellStyles count=\"1\"><cellStyle xfId=\"0\" name=\"Normal\" builtinId=\"0\"/></cellStyles><dxfs count=\"0\"/></styleSheet>";
        file_printer.close();



    }

    SheetExel& operator[](const std::string& nameSheet){
        if (SheetNames.contains(nameSheet))
            return *Sheets[SheetNames[nameSheet]];
        AddNewSheet(nameSheet);
        return *Sheets[SheetNames[nameSheet]];

        //throw std::format("didn't find \"{}\" sheet name", nameSheet);
    }


    static ExelFile* read_CSVs(std::vector<std::string>& filepaths){
        ExelFile* file = new ExelFile;
        for (auto filepath : filepaths){
            if (std::filesystem::is_regular_file(filepath)){
                std::string sheetName=filepath.substr(filepath.find_last_of('/'));
                sheetName=sheetName.substr(0, sheetName.find_last_of('.'));
                if (sheetName.length() && sheetName[0]=='/') sheetName=sheetName.substr(1);
                (*file)[sheetName];
                std::ifstream fileIn(filepath);
                //std::cout << sheetName << "\n";
                std::string buffer;
                for(uint y=1; !fileIn.eof(); y++){
                    getline(fileIn, buffer);
                    if (buffer=="") continue;
                    //std::cout << "\n";
                    auto line=splitBySeparators(buffer, {";"});
                    //std::cout << "endSep\n";
                    for (uint x=1; x<=line.size(); x++){
                        //std::cout << "<" << line[x-1] << "> " ;
                        if (line[x-1]!=""){
                            //
                            //if ()
                            *(*file)[sheetName][{x,y}]=line[x-1];
                        }
                    }
                    //std::cout << (*file)[sheetName].GridSize.first << "x" << (*file)[sheetName].GridSize.second  << "\n";
                    //std::cout << "\nend\n\n";

                }
            }
        }
        return file;
    }


    std::map<std::string, uint> SheetNames;

private:
    static inline std::vector<std::string> splitBySeparators(std::string& line, std::vector<std::string> separators){
        std::vector<std::string> answer;
        uint pos=0;
        while(line!=""){
            //std::cout << line << "\n";
            for (auto now: separators){
                pos=line.find(now);
                if (pos<line.size()) break;
            }
            answer.push_back(line.substr(0, pos));
            if (pos < line.length()) ++pos;
            line=line.substr(std::min(pos, uint(line.length())));
        }
        return answer;
    }

    void inline AddNewSheet(const std::string& name){
        SheetNames[name]=SheetNames.size();
        Sheets.push_back(new SheetExel(name, ++Sheet_count));
    }

    void Workbook_print(std::ofstream& fout){
        fout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<workbook xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" xmlns:mx=\"http://schemas.microsoft.com/office/mac/excel/2008/main\" xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" xmlns:mv=\"urn:schemas-microsoft-com:mac:vml\" xmlns:x14=\"http://schemas.microsoft.com/office/spreadsheetml/2009/9/main\" xmlns:x15=\"http://schemas.microsoft.com/office/spreadsheetml/2010/11/main\" xmlns:x14ac=\"http://schemas.microsoft.com/office/spreadsheetml/2009/9/ac\" xmlns:xm=\"http://schemas.microsoft.com/office/excel/2006/main\"><workbookPr/><sheets>";
        auto r_id=3;
        for (auto sheet: Sheets){
            sheet->print_4_workbool_xml(fout, ++r_id);
        }
        fout << "</sheets><definedNames/><calcPr/></workbook>";
    }

    std::vector<SheetExel*> Sheets;
    unsigned int Sheet_count=0;
};


#endif // EXELWORK_H
