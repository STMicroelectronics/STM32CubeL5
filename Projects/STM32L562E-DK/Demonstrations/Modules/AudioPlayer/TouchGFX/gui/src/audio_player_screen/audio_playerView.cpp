#include <gui/audio_player_screen/audio_playerView.hpp>
#include "BitmapDatabase.hpp"

#if !defined(SIMULATOR)
#include "audio_player_app.h"
#endif


static int fileAvailableOnMediaStorage = 1;

#if !defined(SIMULATOR)

/* Audio in external Flash management */
typedef struct
{
  uint32_t   ChunkID;       /* 0 */
  uint32_t   FileSize;      /* 4 */
  uint32_t   FileFormat;    /* 8 */
  uint32_t   SubChunk1ID;   /* 12 */
  uint32_t   SubChunk1Size; /* 16*/
  uint16_t   AudioFormat;   /* 20 */
  uint16_t   ChannelsNbr;   /* 22 */
  uint32_t   SampleRate;    /* 24 */

  uint32_t   ByteRate;      /* 28 */
  uint16_t   BlockAlign;    /* 32 */
  uint16_t   BitsPerSample; /* 34 */
  uint32_t   SubChunk2ID;   /* 36 */
  uint32_t   SubChunk2Size; /* 40 */

} WAVE_FILE_FormatTypeDef;

/* Audio file size and start address are defined here since the Audio file is
   stored in Flash memory as a constant table of 16-bit data */
#define AUDIO_FILE_SIZE      0x2f9c9e  /* from MAP file */
#define PLAY_HEADER          0x2C
#define PLAY_BUFF_SIZE       4096

int16_t       *waveFilePlayBuff = NULL;
__IO uint32_t waveFilePlaybackPosition;

uint32_t AudioPlayTest = 0;

#endif

// symbol 'audio_bin_file' defined by linker option --image_input
extern uint8_t audio_bin_file[];

audio_playerView::audio_playerView() :
    updateTrackListCallback(this, &audio_playerView::updateTrackListHandler),
    trackListItemSelectedCallback(this, &audio_playerView::trackListItemSelectedHandler),
    sliderIsMoved(false),
    activeTrack(0)
{

}

void audio_playerView::setupScreen()
{
    audio_playerViewBase::setupScreen();
    trackSelectorBox.setUpdatePlaylist(updateTrackListCallback);
    trackSelectorBox.setSelectedItemCallback(trackListItemSelectedCallback);
    musicDurationSlider.setTouchable(false);
    durationTotal.setTouchable(false);
}

void audio_playerView::tearDownScreen()
{
    audio_playerViewBase::tearDownScreen();
}

void audio_playerView::handleClickEvent(const ClickEvent& evt)
{
    if (musicDurationSlider.getRect().intersect(Rect(evt.getX(), evt.getY(), 1, 1)) && evt.getType() == ClickEvent::PRESSED)
    {
        sliderIsMoved = true;
    }
    else
    {
        sliderIsMoved = false;
    }

    audio_playerViewBase::handleClickEvent(evt);
}

void audio_playerView::updateMusicVolume(int volume)
{
    volumeSlider.setValue(volume);
    volumeSliderChanged(volume);
    if (volumeBarContainer.getX() != VOLUME_SLIDER_X)
    {
        volumeButton.forceState(false);
        volumeButton.invalidate();
    }
}

void audio_playerView::volumeButtonPressed(bool value)
{
    if (trackSelectorBox.getX() < HAL::DISPLAY_WIDTH)
    {
        trackSelectorBox.startMoveAnimation(HAL::DISPLAY_WIDTH, trackSelectorBox.getY(), 18, touchgfx::EasingEquations::cubicEaseIn, touchgfx::EasingEquations::cubicEaseIn);
        volumeBarContainer.setMoveAnimationDelay(20);
        cancelButton.setVisible(false);
        albumButton.setVisible(true);
        cancelButton.invalidate();
        albumButton.invalidate();
    }
    else
    {
        volumeBarContainer.setMoveAnimationDelay(0);
    }
    if (value)
    {
        volumeBarContainer.startMoveAnimation(VOLUME_SLIDER_X, volumeBarContainer.getY(), VOLUME_SLIDER_MOVE_DURATION, EasingEquations::cubicEaseOut);
    }
    else
    {
        volumeBarContainer.startMoveAnimation(HAL::DISPLAY_WIDTH, volumeBarContainer.getY(), VOLUME_SLIDER_MOVE_DURATION, EasingEquations::cubicEaseOut);
    }
}

void audio_playerView::albumButtonPressed()
{
    if (volumeBarContainer.getX() < HAL::DISPLAY_WIDTH)
    {
        volumeBarContainer.startMoveAnimation(HAL::DISPLAY_WIDTH, volumeBarContainer.getY(), 18, touchgfx::EasingEquations::cubicEaseIn, touchgfx::EasingEquations::cubicEaseIn);
        trackSelectorBox.setMoveAnimationDelay(20);
        volumeButton.forceState(false);
        volumeButton.invalidate();
    }
    else
    {
        trackSelectorBox.setMoveAnimationDelay(0);
    }
    trackSelectorBox.startMoveAnimation(50, trackSelectorBox.getY(), 18, touchgfx::EasingEquations::cubicEaseIn, touchgfx::EasingEquations::cubicEaseIn);
    cancelButton.setVisible(true);
    cancelButton.invalidate();
}

void audio_playerView::volumeSliderChanged(uint16_t value)
{
    presenter->setMusicVolume(value);

    if (value == 0)
    {
        volumeButton.setBitmaps(Bitmap(BITMAP_ICON_VOLUME_MUTED_ID), Bitmap(BITMAP_ICON_VOLUME_MUTED_PRESSED_ID));
    }
    else if (value <= 33)
    {
        volumeButton.setBitmaps(Bitmap(BITMAP_ICON_VOLUME_LOW_ID), Bitmap(BITMAP_ICON_VOLUME_LOW_PRESSED_ID));
    }
    else if (value <= 66)
    {
        volumeButton.setBitmaps(Bitmap(BITMAP_ICON_VOLUME_MEDIUM_ID), Bitmap(BITMAP_ICON_VOLUME_MEDIUM_PRESSED_ID));
    }
    else
    {
        volumeButton.setBitmaps(Bitmap(BITMAP_ICON_VOLUME_HIGH_ID), Bitmap(BITMAP_ICON_VOLUME_HIGH_PRESSED_ID));
    }

    volumeButton.forceState(true);

    volumeButton.invalidate();
}

void audio_playerView::playPauseButtonPressed(bool value)
{
    presenter->setPlaySong(value);
    setPlayingGraphics(value);
}

void audio_playerView::setPlayingGraphics(bool playing)
{
    trackSelectorBox.refreshTrackList();

    onLed.setVisible(playing);
    onLed.invalidate();

    playPauseAnimation.startAnimation(!playing);

    playPauseButton.forceState(playing);
    playPauseButton.invalidate();
}

void audio_playerView::updateMusicDuration(int totalDuration, int minutes, int seconds)
{
    if (!sliderIsMoved)
    {
        musicDurationSlider.setValue(totalDuration);
        musicDurationSlider.invalidate();
    }

    Unicode::snprintf(durationLeftBuffer, DURATIONLEFT_SIZE, "%.2d:%.2d", minutes, seconds);
    durationLeft.invalidate();

    currentSongMinutes = minutes;
    currentSongSeconds = seconds;

    if (trackSelectorBox.getX() < HAL::DISPLAY_HEIGHT)
    {
        trackSelectorBox.refreshTrackList();
    }
}

void audio_playerView::setMusicDuration(int totalLength, int minutes, int seconds)
{
    musicDurationSlider.setValueRange(0, totalLength);
    musicDurationSlider.invalidate();

    Unicode::snprintf(durationTotalBuffer, DURATIONTOTAL_SIZE, "%.2d:%.2d", minutes, seconds);
    durationTotal.invalidate();
}

void audio_playerView::musicSliderChanged(uint16_t value)
{
    presenter->setSongDuration(value);
    presenter->updateMusicDuration(value);
}

void audio_playerView::updateTrackListHandler(track& currentTrack, int trackId)
{
    touchgfx::Unicode::UnicodeChar songName[30];
    presenter->getTrackName(songName, trackId);
    currentTrack.setTrackName(songName);

    currentTrack.setTrackNumber(trackId+1);
    currentTrack.setSelectedTrack(presenter->getSelected(trackId));

    if (presenter->getSelected(trackId))
    {
        currentTrack.setTrackLengthDuration(currentSongMinutes, currentSongSeconds);
        currentTrack.setTrackPlaying(presenter->getPlaySong());
    }
    else
    {
        currentTrack.setTrackLengthDuration(presenter->getTrackLengthMinutes(trackId), presenter->getTrackLengthSeconds(trackId));
    }
}

void audio_playerView::setTrackListSize(int size)
{
    trackSelectorBox.setTrackListSize(size);
}

void audio_playerView::refreshTrackList()
{
    trackSelectorBox.refreshTrackList();
}

void audio_playerView::trackListItemSelectedHandler(int item)
{
    presenter->setSelected(false, presenter->getCurrentSong());
    presenter->setSelected(true, item);
    presenter->setCurrentSong(item);

    trackSelectorBox.refreshTrackList();
}

void audio_playerView::trackListPlayPausePressed(bool value)
{
    playPauseButtonPressed(value);
    playPauseButton.forceState(value);
    playPauseButton.invalidate();
}

void audio_playerView::setActiveSong(int song)
{
    presenter->setCurrentSong(song);
}

void audio_playerView::nextSongPressed()
{
    presenter->nextSongPressed();
}

void audio_playerView::previousSongPressed()
{
    presenter->previousSongPressed();
}

void audio_playerView::shufflePressed(bool value)
{
    presenter->setShuffle(value);

    trackSelectorBox.setRepeatButtonState(false);
    presenter->setRepeat(false);
}

void audio_playerView::repeatPressed(bool value)
{
    presenter->setRepeat(value);

    trackSelectorBox.setShuffleButtonState(false);
    presenter->setShuffle(false);
}

void audio_playerView::psCountUpdate(int cnt)
{
    Unicode::snprintf(shuttingOfCounterBuffer1, SHUTTINGOFCOUNTERBUFFER1_SIZE, "%d", cnt);

    if (cnt == 1)
    {
        Unicode::snprintf(shuttingOfCounterBuffer2, SHUTTINGOFCOUNTERBUFFER2_SIZE, "SECOND");
    }
    else
    {
        Unicode::snprintf(shuttingOfCounterBuffer2, SHUTTINGOFCOUNTERBUFFER2_SIZE, "SECONDS");
    }

    if (cnt == 0)
    {
        powerSaveContainer.setVisible(false);
        powerSaveContainer.invalidate();

        cancalPowerSavePressed();
    }

    shuttingOfCounter.invalidate();
}

void audio_playerView::cancalPowerSavePressed()
{
    presenter->cancelPowerSave();
}

void audio_playerView::powerSavePressed()
{
    Unicode::snprintf(shuttingOfCounterBuffer1, SHUTTINGOFCOUNTERBUFFER1_SIZE, "%d", 5);
    Unicode::snprintf(shuttingOfCounterBuffer2, SHUTTINGOFCOUNTERBUFFER2_SIZE, "SECONDS");
    shuttingOfCounter.invalidate();

    presenter->powerSavePressed();
}

void audio_playerView::backButtonPressed()
{
    vPortFree(waveFilePlayBuff);
    waveFilePlayBuff = NULL;

    AudioPlayTest = 0;

    presenter->returnToMenuPressed();
}

void audio_playerView::noMediaFilesDetected(bool filesDetected)
{
    background.setVisible(filesDetected);
    bottomBar.setVisible(filesDetected);
    playPauseAnimation.setVisible(filesDetected);

    background.invalidate();
    bottomBar.invalidate();
    playPauseAnimation.invalidate();

    continuousPlaybackFromFlash(filesDetected);
}

void audio_playerView::continuousPlaybackFromFlash(bool filesDetected)
{
    fileAvailableOnMediaStorage = filesDetected ? 1 : 0;
    if (!fileAvailableOnMediaStorage)
    {
        // Start Playback from Flash
        /* Check if the audio buffer has been loaded in flash */
        if (*((uint64_t *)audio_bin_file) != 0x017EFE2446464952 )
        {
            uint8_t status = 0;
            BSP_AUDIO_Init_t AudioInit = {0};
            /* Retrieve Wave Sample rate*/
            WAVE_FILE_FormatTypeDef *waveFileFormat =  (WAVE_FILE_FormatTypeDef*) audio_bin_file;

            waveFilePlaybackPosition   = (2*PLAY_BUFF_SIZE) + PLAY_HEADER;

            /* Allocate memory for audio playback */
            waveFilePlayBuff = (int16_t*)pvPortMalloc(PLAY_BUFF_SIZE);
            if (waveFilePlayBuff)
            {
                /* Initialize Audio Device */
                AudioInit.Device        = AUDIO_OUT_HEADPHONE;
                AudioInit.SampleRate    = waveFileFormat->SampleRate;
                AudioInit.BitsPerSample = waveFileFormat->BitsPerSample;
                AudioInit.ChannelsNbr   = waveFileFormat->ChannelsNbr;
                AudioInit.Volume        = 80;
                BSP_AUDIO_OUT_DeInit(0);
                if (BSP_AUDIO_OUT_Init(0, &AudioInit) != BSP_ERROR_NONE)
                {
                  status = 1;
                }
                else
                {
                    /* Initialize the data buffer */
                    for(int i=0; i < PLAY_BUFF_SIZE; i++)
                    {
                        waveFilePlayBuff[i]=*((__IO uint16_t *)(audio_bin_file + PLAY_HEADER + (2*i)));
                    }

                    /* Start the audio player */
                    if (BSP_AUDIO_OUT_Play(0, (uint8_t *) waveFilePlayBuff, 2*PLAY_BUFF_SIZE) != BSP_ERROR_NONE)
                    {
                      status = 1;
                    }
                 }
                 AudioPlayTest = (status == 0) ? 1 : 0;
            }
        }
    }
}

#if !defined(SIMULATOR)
/**
 * @brief  Manages the DMA full Transfer complete event
 * @param  Instance:Instance AUDIO OUT Instance. It can only be 0 (SAI) or 1 (I2S)
 * @retval None
 */
void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Instance)
{
    if(fileAvailableOnMediaStorage)
    {
        AUDIOPLAYER_TransferComplete_CallBack();
    }
    else
    {
      if ((Instance == 0U) && (AudioPlayTest == 1))
      {
          /* Upate the first or the second part of the buffer */
          for(int i = 0; i < PLAY_BUFF_SIZE/2; i++)
          {
              waveFilePlayBuff[i+ (PLAY_BUFF_SIZE/2)] = *(uint16_t *)(audio_bin_file + waveFilePlaybackPosition);
              waveFilePlaybackPosition+=2;
          }

          /* check the end of the file */
          if((waveFilePlaybackPosition+PLAY_BUFF_SIZE/2) > AUDIO_FILE_SIZE)
          {
              waveFilePlaybackPosition = PLAY_HEADER;
          }
      }
    }
}


/**
 * @brief  Manages the DMA Half Transfer complete event
 * @param  Instance:Instance AUDIO OUT Instance. It can only be 0 (SAI) or 1 (I2S)
 * @retval None
 */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Instance)
{
    if(fileAvailableOnMediaStorage)
    {
        AUDIOPLAYER_HalfTransfer_CallBack();
    }
    else
    {
        /* Upate the first or the second part of the buffer */
        for(int i = 0; i < PLAY_BUFF_SIZE/2; i++)
        {
          waveFilePlayBuff[i] = *(uint16_t *)(audio_bin_file + waveFilePlaybackPosition);
          waveFilePlaybackPosition+=2;
        }

        /* check the end of the file */
        if((waveFilePlaybackPosition+PLAY_BUFF_SIZE/2) > AUDIO_FILE_SIZE)
        {
          waveFilePlaybackPosition = PLAY_HEADER;
        }
    }
}
#endif
