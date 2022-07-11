#ifdef EXPORT_EXPORTS
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __declspec(dllexport)
#endif

class EXPORT_API CExport {
public:
	CExport(void);
};

extern EXPORT_API char nExport;
extern "C" EXPORT_API void fnExport(const char *pImgPath,const char *pLayoutInfo,const char *pImgSavePath);