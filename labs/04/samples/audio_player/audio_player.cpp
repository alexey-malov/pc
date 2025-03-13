#include "miniaudio.h"

#include <cmath>
#include <functional>
#include <iostream>
#include <numbers>
#include <span>
#include <string>

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

class Player
{
public:
	using DataCallback = std::function<void(void* output, ma_uint32 frameCount)>;

	Player(ma_format format, ma_uint32 channels, ma_uint32 sampleRate = 48000)
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

class SineWaveGenerator
{
public:
	SineWaveGenerator(ma_uint32 sampleRate, ma_float frequency, ma_float amplitude = 1.f, ma_float startPhase = 0.f)
		: m_sampleRate{ sampleRate }
		, m_frequency{ frequency }
		, m_amplitude{ amplitude }
		, m_phase{ startPhase }
	{
	}
	ma_float GetNextSample()
	{
		constexpr auto twoPi = static_cast<ma_float>(2.f * std::numbers::pi);

		auto sample = m_amplitude * std::sin(m_phase);
		m_phase = std::fmod(m_phase + m_phaseShift, twoPi);

		return sample;
	}

private:
	ma_uint32 m_sampleRate;
	ma_float m_frequency;
	ma_float m_amplitude;
	ma_float m_phase;
	// На сколько смещается фаза синусоидальных колебаний при переходе к следующему отсчёту
	ma_float m_phaseShift = static_cast<ma_float>(2.f * std::numbers::pi * m_frequency / m_sampleRate);
};

} // namespace audio

int main()
{
	audio::Player player(ma_format_f32, 1);

	// Частота звука, который выше на 1 полутон, выше в корень 12 степени из 2 раз.
	// Частота звука, который выше на 1 октаву (12 полутонов) выше в 2 раза

	// Аккорд Ля минор A4-C5-E5
	// Ля 1 октавы
	audio::SineWaveGenerator a4{ player.GetSampleRate(), 440.0f, 0.3f }; // Ля 1 октавы
	// Нота До 2 октавы (на 3 полутона выше, чем Ля 1 октавы)
	audio::SineWaveGenerator c5{ player.GetSampleRate(), 440.0f * std::powf(2.0f, 3.0f / 12.f), 0.3f };
	// Нота Ми 2 октавы (на 7 полутонов выше, чем Ля 1 октавы)
	audio::SineWaveGenerator e5{ player.GetSampleRate(), 440.0f * std::powf(2.0f, 7.0f / 12.f), 0.3f };

	player.SetDataCallback([&a4, &c5, &e5](void* output, ma_uint32 frameCount) mutable {
		// Заполняем буфер аудио-сэмплами
		auto samples = std::span(static_cast<ma_float*>(output), frameCount);
		for (auto& sample : samples)
		{
			// Складываем сэмплы со всех генераторов аудио-сигнала
			sample = a4.GetNextSample();
			sample += c5.GetNextSample();
			sample += e5.GetNextSample();
		}
	});

	player.Start();

	std::string s;
	std::getline(std::cin, s);
}
