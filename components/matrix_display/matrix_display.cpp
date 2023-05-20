#include "matrix_display.h"

namespace esphome
{
  namespace matrix_display
  {

    static const char *const TAG = "MatrixDisplay";

    /**
     * Initialize the wrapped matrix display with user parameters
     */
    void MatrixDisplay::setup()
    {
      ESP_LOGCONFIG(TAG, "Setting up MatrixDisplay...");

      // Module configuration
      HUB75_I2S_CFG mxconfig(
          panel_width_,  // module width
          panel_height_, // module height
          chain_length_, // Chain length
          pins_);

      // mxconfig.clkphase = false;
      mxconfig.driver = HUB75_I2S_CFG::FM6124;

      mxconfig.double_buff = true;

      // Display Setup
      dma_display_ = new MatrixPanel_I2S_DMA(mxconfig);
      dma_display_->begin();
      set_brightness(initial_brightness_);
      dma_display_->clearScreen();

      // Default to off if power switches are present
      set_state(!power_switches_.size());
    }

    /**
     * Updates the displayed image on the matrix. Dual buffers are used to prevent blanking in-between frames.
     */
    void MatrixDisplay::update()
    {
      if (enabled_)
      {
        // Draw updates to the screen
        this->do_update_();
      }
      else
      {
        dma_display_->clearScreen();
      }
      // Flip buffer to show changes
      dma_display_->flipDMABuffer();
    }

    void MatrixDisplay::dump_config()
    {
      ESP_LOGCONFIG(TAG, "MatrixDisplay:");
      ESP_LOGCONFIG(TAG, "Pins: R1:%i, G1:%i, B1:%i, R2:%i, G2:%i, B2:%i", pins_.r1, pins_.g1, pins_.b1, pins_.r2, pins_.g2, pins_.b2);
      ESP_LOGCONFIG(TAG, "Pins: A:%i, B:%i, C:%i, D:%i, E:%i", pins_.a, pins_.b, pins_.c, pins_.d, pins_.e);
      ESP_LOGCONFIG(TAG, "Pins: LAT:%i, OE:%i, CLK:%i", pins_.lat, pins_.oe, pins_.b1, pins_.clk);
    }

    void MatrixDisplay::set_state(bool state)
    {
      enabled_ = state;
    }

    void MatrixDisplay::set_brightness(int brightness)
    {
      // Wrap brightness function
      dma_display_->setBrightness8(brightness);
    }

    void HOT MatrixDisplay::draw_absolute_pixel_internal(int x, int y, Color color)
    {
      // Reject invalid pixels
      if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0)
        return;

      // Update pixel value in buffer
      dma_display_->drawPixelRGB888(x, y, color.r, color.g, color.b);
    }

    void MatrixDisplay::fill(Color color)
    {
      // Wrap fill screen method
      dma_display_->fillScreenRGB888(color.r, color.g, color.b);
    }

    void MatrixDisplay::filled_rectangle(int x1, int y1, int width, int height, Color color)
    {
      // Wrap fill rectangle method
      dma_display_->fillRect(x1, y1, width, width, color.r, color.g, color.b);
    }

  } // namespace matrix_display
} // namespace esphome
