#ifndef anapi_asset_manager_h_included
#define anapi_asset_manager_h_included

#include <android/native_activity.h>
#include <android/asset_manager.h>

namespace anapi
{

	enum struct seek_position
	{ from_start, from_cur, from_end };

	class asset
	{
	private:
		struct description
		{
			int fd;
			off_t start;
			off_t length;
		};
	private:
		JNIEnv *m_jni;
		AAssetManager *m_am;
		description m_desc;

		asset(JNIEnv *jni, AAssetManager *am);
		void close();

		friend class asset_manager;

	public:
		asset();
		~asset();

		asset(asset&& other);
		asset& operator=(asset&& other);

		bool open(const std::string& name);
		ssize_t read(void *p, size_t length) const;

		bool seek(off_t offset, const seek_position& whence = seek_position::from_start) const;

	private:
		asset(const asset& other) = delete;
		asset& operator=(const asset& other) = delete;
	};

	class asset_manager
	{
	private:
		JNIEnv *m_jni;
		AAssetManager *m_am;

	public:
		asset_manager();
		asset_manager(JNIEnv *jni, AAssetManager *am);
		~asset_manager();

		asset_manager(const asset_manager& other);
		asset_manager& operator=(const asset_manager& other);
		asset_manager(asset_manager&&);
		asset_manager& operator=(asset_manager&&);

		asset make_asset() const;
	};

}

#endif // anapi_asset_manager_h_included
