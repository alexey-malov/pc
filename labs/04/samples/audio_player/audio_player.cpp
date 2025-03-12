#include "miniaudio.h"

#include <functional>
#include <iostream>
#include <span>
#include <string>
#include <cmath>
#include <numbers>

namespace audio
{

inline const std::error_category& ErrorCategory()
{
	class AudioErrorCategory : public std::error_category
	{
	public:
		const char* name() const noexcept override
		{
			return "Audio error category";
		}

		std::string message(int errorCode) const override
		{
			return ma_result_description(static_cast<ma_result>(errorCode));
		}
	};

	static AudioErrorCategory errorCategory;
	return errorCategory;
}

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

	void Start()
	{
		if (auto result = ma_device_start(&m_device); result != MA_SUCCESS)
		{
			throw std::system_error(result, ErrorCategory());
		}
	}

	void Stop()
	{
		if (auto result = ma_device_stop(&m_device); result != MA_SUCCESS)
		{
			throw std::system_error(result, ErrorCategory());
		}
	}

	ma_device* operator->() noexcept
	{
		return &m_device;
	}

	const ma_device* operator->() const noexcept
	{
		return &m_device;
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

class SoundGenerator
{
public:
	using DataCallback = std::function<void(void* output, ma_uint32 frameCount)>;

	SoundGenerator(ma_format format, ma_uint32 channels, ma_uint32 sampleRate = 48000)
		: m_device(CreateConfig(format, channels, sampleRate))
	{
		m_device.SetDataCallback([this](void* output, const void*, ma_uint32 frameCount) {
			if (m_dataCallback)
			{
				m_dataCallback(output, frameCount);
			}
		});
	}

	void Start()
	{
		m_device.Start();
	}

	void Stop()
	{
		m_device.Stop();
	}

	ma_format GetFormat() const noexcept
	{
		return m_device->playback.format;
	}

	ma_uint32 GetChannels() const noexcept
	{
		return m_device->playback.channels;
	}

	ma_uint32 GetSampleRate() const noexcept
	{
		return m_device->sampleRate;
	}

	void SetDataCallback(DataCallback callback)
	{
		m_dataCallback = std::move(callback);
	}

private:
	static ma_device_config CreateConfig(ma_format format, ma_uint32 channels, ma_uint32 sampleRate)
	{
		auto config = ma_device_config_init(ma_device_type_playback);
		auto& playback = config.playback;

		playback.format = format;
		playback.channels = channels;
		config.sampleRate = sampleRate;

		return config;
	}

	DataCallback m_dataCallback;
	Device m_device;
};
} // namespace audio

int main()
{
	audio::SoundGenerator generator(ma_format_f32, 1);
	// Частота ноты Ля 1 октавы
	constexpr double frequency = 440.0;
	const auto period = generator.GetSampleRate() / frequency;

	generator.SetDataCallback([phase = 0.0, period](void* output, ma_uint32 frameCount) mutable {
		auto samples = std::span(static_cast<ma_float*>(output), frameCount);
		for (auto& sample : samples)
		{
			// Синусоидальный сигнал (чистый тон)
			sample = static_cast<ma_float>(std::sin(phase * 2 * std::numbers::pi / period));

			if ((phase += 1) >= period)
			{
				phase -= period;
			}
		}
	});

	generator.Start();

	std::string s;
	std::getline(std::cin, s);
}
