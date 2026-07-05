#pragma once

#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <chrono>
#include <vector>
#include <fstream>
#include <mutex>

#include "bigcomplex.h"
#include "palette.h"
#include "Photo.hpp"

struct RenderUnit
{
	int32_t outIteration;
	double norm;
	bool completed;
};

class RenderResult
{
public:
	RenderResult();
	
	void setSize(int32_t, int32_t);
	void setMaxIterationCount(int32_t);
	void restart();

	int32_t getWidth() const;
	int32_t getHeight() const;
	int32_t getMaxIterationCount() const;

	RenderUnit& getUnit(int32_t, int32_t);
	void fillBlock(int32_t, int32_t, int32_t, int32_t, RenderUnit);

protected:
	std::vector<RenderUnit> render;
	int32_t width;
	int32_t height;
	int32_t maxIterationCount;

	RenderUnit _buff;
};

class MandelbrotGenerator
{
public:
	MandelbrotGenerator();

	void setScreenSize(int32_t, int32_t);
	void setMaxPixelIterationCount(int32_t);
	void setCenterPosition(const BigFloat&, const BigFloat&);
	void setZoom(const BigFloat&);
	void setPrecision(int32_t);
	void setPalette(int32_t);

	void saveDump(const std::string&);
	void loadDump(const std::string&);

	void regenerate();
	int32_t getPixelIterationCount();
	int64_t getImageIterationCount();
	int32_t getCurrentRenderStep();
	int64_t getCurrentImageIteration();
	int32_t getPrecision();
	int32_t getRenderStepCount();
	BigFloat getZoom();
	BigComplex getCenterPosition();
	BigFloat getScreenWidth();
	BigFloat getScreenHeight();

	void changePause(bool);

	std::chrono::time_point<std::chrono::steady_clock> getStartTime();
	std::chrono::time_point<std::chrono::steady_clock> getFinishTime();

	void renderStep();

	void draw(Photo*);

protected:
	COLORREF getPixelColor(int32_t, double);

	RenderResult renderResult;

	BigComplex centerPos;
	BigFloat zoom;
	BigFloat screenWidth;
	BigFloat screenHeight;

	int64_t currentImageIteration;
	int32_t currentStep;
	int32_t renderStepCount;
	int32_t pixelSize;
	int32_t precision;
	int32_t palette;

	int32_t thPos;

	std::chrono::time_point<std::chrono::steady_clock> renderStart;
	std::chrono::time_point<std::chrono::steady_clock> renderFinish;

	std::mutex pause;
};

#endif // !MANDELBROT_H
