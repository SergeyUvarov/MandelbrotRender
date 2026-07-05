#include "mandelbrotgenerator.h"

RenderResult::RenderResult() : width(0), height(0), maxIterationCount(0), render(), _buff()
{

}

void RenderResult::setSize(int32_t w, int32_t h)
{
	if (w > 0 && h > 0) {
		render.resize(w * h);
		width = w;
		height = h;
		//restart();
	}
}

void RenderResult::setMaxIterationCount(int32_t iterCount)
{
	if (iterCount > 0)
	{
		for (auto& unit : render)
			if(unit.outIteration == maxIterationCount)
				unit.completed = false;
		maxIterationCount = iterCount;
	}
}

void RenderResult::restart()
{
	for (auto& unit : render)
		unit.completed = false;
}

int32_t RenderResult::getWidth() const
{
	return width;
}

int32_t RenderResult::getHeight() const
{
	return height;
}

int32_t RenderResult::getMaxIterationCount() const
{
	return maxIterationCount;
}

RenderUnit& RenderResult::getUnit(int32_t x, int32_t y)
{
	if (render.size() == 0) return _buff;
	if (x >= 0 && y >= 0 && x < width && y < height)
	{
		return render[y * width + x];
	}
	return _buff;
}

void RenderResult::fillBlock(int32_t x0, int32_t y0, int32_t w, int32_t h, RenderUnit src)
{
	src.completed = false;
	for (int32_t y = y0; y < y0 + h; y++) 
	{
		for (int32_t x = x0; x < x0 + w; x++)
		{
			getUnit(x, y) = src;
		}
	}
	getUnit(x0, y0).completed = true;
}

MandelbrotGenerator::MandelbrotGenerator(): renderResult(), centerPos(), zoom("1"), screenWidth(), screenHeight(), renderStepCount(1), currentStep(0), pixelSize(0), precision(0), currentImageIteration(0), palette(0), pause(), thPos(0)
{

}

void MandelbrotGenerator::setScreenSize(int32_t w, int32_t h)
{
	if (w > 1 && h > 1)
	{
		renderResult.setSize(w, h);
		screenWidth = std::to_string(w);
		screenHeight = std::to_string(h);
		renderStepCount = log2(w);
		regenerate();
	}
}

void MandelbrotGenerator::setMaxPixelIterationCount(int32_t iCount)
{
	renderResult.setMaxIterationCount(iCount);
	regenerate();
}

void MandelbrotGenerator::setCenterPosition(const BigFloat& x, const BigFloat& y)
{
	centerPos = BigComplex(x, y);
	renderResult.restart();
	regenerate();
}

void MandelbrotGenerator::setZoom(const BigFloat& z)
{
	zoom = z;
	regenerate();
	renderResult.restart();
}

void MandelbrotGenerator::setPrecision(int32_t p)
{
	precision = p;
	regenerate();
	renderResult.restart();
}

void MandelbrotGenerator::setPalette(int32_t p)
{
	if (p >= 0 && p < 4)
	{
		palette = p;
	}
}

void MandelbrotGenerator::saveDump(const std::string& fileName)
{
	std::ofstream f(fileName, std::ios::out);

	if (!f)
		return;

	changePause(true);

	RenderResult cpy(renderResult);

	f << centerPos.getReal().to_str() << "\n"
		<< centerPos.getImaginary().to_str() << "\n"
		<< zoom.to_str() << "\n"
		<< precision << "\n"
		<< cpy.getWidth() << "\n"
		<< cpy.getHeight() << "\n"
		<< cpy.getMaxIterationCount() << "\n"

		<< getCurrentImageIteration() << "\n"
		<< getCurrentRenderStep() << "\n"
		<< pixelSize << "\n";

	for(int32_t y = 0; y < cpy.getHeight(); y++)
		for (int32_t x = 0; x < cpy.getWidth(); x++)
		{
			RenderUnit& unit = cpy.getUnit(x, y);
			f << unit.outIteration << "," << unit.norm << "," << unit.completed << "\n";
		}

	f.close();

	changePause(false);
}

void MandelbrotGenerator::loadDump(const std::string& fileName)
{
	std::ifstream f(fileName, std::ios::in);

	changePause(true);

	if (!f)
		return;

	std::string sx, sy, sz;
	int32_t w, h, mic;
	f >> sx
		>> sy
		>> sz
		>> precision
		>> w
		>> h
		>> mic;

	setScreenSize(w, h);
	renderResult.setMaxIterationCount(mic);

	f >> currentImageIteration
		>> currentStep
		>> pixelSize;

	centerPos = BigComplex(sx, sy);
	zoom = BigFloat(sz);

	for (int32_t y = 0; y < h; y++)
		for (int32_t x = 0; x < w; x++)
		{
			char buf;
			RenderUnit& unit = renderResult.getUnit(x, y);
			f >> unit.outIteration >> buf >> unit.norm >> buf >> unit.completed;
		}

	f.close();

	changePause(false);
}

void MandelbrotGenerator::regenerate()
{
	currentStep = -1;
	pixelSize = renderResult.getHeight();
	renderStart = std::chrono::steady_clock::now();
	thPos = 0;
}

int32_t MandelbrotGenerator::getPixelIterationCount()
{
	return renderResult.getMaxIterationCount();
}

int64_t MandelbrotGenerator::getImageIterationCount()
{
	if(pixelSize != 0)
		return int64_t(renderResult.getWidth() / pixelSize) * int64_t(renderResult.getHeight() / pixelSize);
	return int64_t(renderResult.getWidth()) * int64_t(renderResult.getHeight());
}

int32_t MandelbrotGenerator::getCurrentRenderStep()
{
	return currentStep;
}

int64_t MandelbrotGenerator::getCurrentImageIteration()
{
	return currentImageIteration;
}

int32_t MandelbrotGenerator::getPrecision()
{
	return precision;
}

int32_t MandelbrotGenerator::getRenderStepCount()
{
	return renderStepCount;
}

BigFloat MandelbrotGenerator::getZoom()
{
	return zoom;
}

BigComplex MandelbrotGenerator::getCenterPosition()
{
	return centerPos;
}

BigFloat MandelbrotGenerator::getScreenWidth()
{
	return screenWidth;
}

BigFloat MandelbrotGenerator::getScreenHeight()
{
	return screenHeight;
}

std::chrono::time_point<std::chrono::steady_clock> MandelbrotGenerator::getStartTime()
{
	return renderStart;
}

std::chrono::time_point<std::chrono::steady_clock> MandelbrotGenerator::getFinishTime()
{
	return renderFinish;
}

COLORREF MandelbrotGenerator::getPixelColor(int32_t n, double norm)
{
	if (n == renderResult.getMaxIterationCount())
		return sRGB(0, 0, 0);

	double v = (n % 100 - log2( log(sqrt(norm)) / log(100) )) / 100.0;

	return getPalette((v > 0) ? v : 1.0, palette);
}

void MandelbrotGenerator::renderStep()
{
	const BigFloat halfWidth = screenWidth * BigFloat("-0.5");
	const BigFloat halfHeght = screenHeight * BigFloat("-0.5");
	const BigFloat four("4");

	currentImageIteration = 0;

	thPos = 0;
	std::mutex thPosMut;
	int32_t thSize = (pixelSize > 16) ? pixelSize : 16;
	auto oneThreadGeneration = [&](int32_t thId, int32_t thCount)
		{
			while (thPos < (renderResult.getHeight() / thSize) * (renderResult.getWidth() / thSize))
			{
				thPosMut.lock();
				int32_t thX = thSize * (thPos % (renderResult.getWidth() / thSize));
				int32_t thY = thSize * (thPos / (renderResult.getWidth() / thSize));
				thPos++;
				thPosMut.unlock();

				for (int32_t y = thY; y < thY + thSize; y += pixelSize)
				{
					for (int32_t x = thX; x < thX + thSize; x += pixelSize)
					{
						if (currentStep == -1) return;
						currentImageIteration++;

						RenderUnit unit = renderResult.getUnit(x, y);

						if (unit.completed)
							continue;

						BigFloat pX = std::to_string(x);
						BigFloat pY = std::to_string(y);

						pX += halfWidth;
						pY += halfHeght;

						pX *= zoom;
						pY *= zoom;

						BigComplex c(pX, pY), z;
						c += centerPos;

						c.setPrecision(precision);
						z.setPrecision(precision);

						unit.outIteration = renderResult.getMaxIterationCount();
						for (int32_t i = 0; i < renderResult.getMaxIterationCount(); i++)
						{
							if (currentStep == -1) return;

							pause.lock();
							pause.unlock();

							z *= z;
							z += c;

							BigFloat norm2 = z.norm2();
							if (norm2 > four)
							{
								unit.outIteration = i;
								unit.norm = norm2.to_double();
								break;
							}
						}

						renderResult.fillBlock(x, y, pixelSize, pixelSize, unit);
					}
				}
			}
		};


	if (getCurrentRenderStep() < getRenderStepCount()) {
		std::thread renderThreads[16];
		for (int32_t i = 0; i < 12; i++)
			renderThreads[i] = std::thread(oneThreadGeneration, i, 12);

		for (int32_t i = 0; i < 12; i++)
			renderThreads[i].join();

		pixelSize /= 2;
		currentStep++;

	}

	if (pixelSize < 1)
	{
		currentImageIteration = getImageIterationCount();
		renderFinish = std::chrono::steady_clock::now();
	}
}

void MandelbrotGenerator::draw(Photo* canvas)
{
	if (canvas == nullptr || renderResult.getWidth() < 1 || renderResult.getHeight() < 1) return;

	int32_t dp = pixelSize;
	if (pixelSize == 0)
		dp = 1;

	for (int32_t y = 0; y < renderResult.getHeight() && y < canvas->height; y += dp)
	{
		for (int32_t x = 0; x < renderResult.getWidth() && y < canvas->width; x += dp)
		{
			RenderUnit &unit = renderResult.getUnit(x, y);

			if (palette == 3)
			{
				if(unit.completed)
					canvas->fillRect(x, y, dp, sRGB(255,255,255));
				else
					canvas->fillRect(x, y, dp, sRGB(0, 0, 0));
			}
			else
				canvas->fillRect(x, y, dp, getPixelColor(unit.outIteration, unit.norm));
		}
	}
}

void MandelbrotGenerator::changePause(bool stat)
{
	if (stat)
	{
		pause.lock();
	}
	else
	{
		pause.unlock();
	}
}