#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <functional>
#include <iostream>

namespace audio
{

class Device
{
public:
	using DataCallback = std::function<void(void*, const void*, ma_uint32)>;

	explicit Device(ma_device_config config)
	{
		config.pUserData = this;
		config.dataCallback = [](ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
			static_cast<Device*>(device->pUserData)->OnDataCallback(output, input, frameCount);
		};

		if (ma_device_init(nullptr, &config, &m_device) != MA_SUCCESS)
		{
			throw std::runtime_error("Device initialization failed");
		}
	}

	void SetDataCallback(DataCallback dataCallback)
	{
		m_dataCallback = std::move(dataCallback);
	}

	Device(const Device&) = delete;
	Device& operator=(const Device&) = delete;

	~Device()
	{
		ma_device_uninit(&m_device);
	}

private:
	void OnDataCallback(void* output, const void* input, ma_uint32 frameCount)
	{
		if (m_dataCallback)
			m_dataCallback(output, input, frameCount);
	}

	ma_device m_device{};
	DataCallback m_dataCallback;
};

class Player
{
public:
	Player(ma_format format, int channels)
		: m_device(CreateConfig(format, channels))
	{
	}

private:
	static ma_device_config CreateConfig(ma_format format, int channels)
	{
		auto config = ma_device_config_init(ma_device_type_playback);
		auto& playback = config.playback;

		playback.format = format;
		playback.channels = channels;
		config.sampleRate = 44100;
		return config;
	}

	Device m_device;
};
} // namespace audio

int main()
{

	return 0;
}
