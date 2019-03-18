uint8_t __attribute__((always_inline)) rng(){
  zx++;
  za = (za^zc^zx);
  zb = (zb+za);
  zc = (zc+(zb>>1)^za);
  return zc;
}

void updateStars(){
  uint8_t spawnDepthVariation = 255;
  for(int i = 0; i < NSTARS; ++i){
    if (sz[i] <= 1)
    {
      sy[i] = 160 - 120 + rng();
      sx[i] = rng();
      sz[i] = spawnDepthVariation--;
    }
    else
    {
      int old_screen_x = ((int)sx[i] - 120) * 256 / sz[i] + 120;
      int old_screen_y = ((int)sy[i] - 160) * 256 / sz[i] + 160;

      if(old_screen_x<starMinKeepOutX || old_screen_x>starMaxKeepOutX || old_screen_y<starMinKeepOutY || old_screen_y>starMaxKeepOutY){
        tft.drawPixel(old_screen_x, old_screen_y,TFT_BLACK);
      }

      sz[i] -= 2;
      if (sz[i] > 1)
      {
        int screen_x = ((int)sx[i] - 120) * 256 / sz[i] + 120;
        int screen_y = ((int)sy[i] - 160) * 256 / sz[i] + 160;
  
        if (screen_x >= 0 && screen_y >= 0 && screen_x < 240 && screen_y < 320)
        {
          if(screen_x<starMinKeepOutX || screen_x>starMaxKeepOutX || screen_y<starMinKeepOutY || screen_y>starMaxKeepOutY){
            uint8_t r, g, b;
            if(state==STATE_CHALLENGED || state==STATE_CHOOSE_OPPONENT){
              g=b=31;
              r = 255;
            }else{
              r = g = b = 255 - sz[i];
            }
            tft.drawPixel(screen_x, screen_y, tft.color565(r,g,b));
          }
        }
        else
          sz[i] = 0; // Out of screen, die.
      }
    }
  }
}
