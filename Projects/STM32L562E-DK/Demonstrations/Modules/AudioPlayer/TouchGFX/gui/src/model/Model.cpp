#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if !defined(SIMULATOR)
#include "storage.h"
#include "audio_player_app.h"
#if defined(PM_SUPPORT)
#include "audio_lowpower.h"
#endif

#if defined(FUNCTIONAL_PART_AVAILABLE) && defined(PM_SUPPORT)
extern osMessageQId   LPModeEvent;
#if defined(PM_SUPPORT)
extern __IO uint16_t GUITaskResumed;
#else
__IO uint16_t GUITaskResumed = 0x1;
#endif
#endif
#endif

Model::Model() : modelListener(0), musicVolume(70), songLength(524), songDuration(0), playSong(false), psCountDown(false), numberOfSongs(10), repeat(false), shuffle(false), tickCnt(0), psTicks(0), psCountDownCnt(5)
{

}

void Model::tick()
{
    if (playSong)
    {
        if (tickCnt == TICK_COUNT)
        {
            if (songDuration == songLength)
            {
                if (shuffle)
                {
                    setSelected(false, getCurrentSong());
                    setSelected(true, rand() % numberOfSongs);
                }
                else
                {
                    setSelected(false, getCurrentSong());

                    if (getCurrentSong() == numberOfSongs - 1)
                    {
                        modelListener->setPlayingGraphics(repeat);
                        playSong = repeat;
                        setSelected(true, 0);
                    }
                    else
                    {
                        setSelected(true, getCurrentSong() + 1);
                    }

                }
            }
            else
            {
#if !defined(SIMULATOR) && defined(FUNCTIONAL_PART_AVAILABLE) && defined(PM_SUPPORT)
              if(GUITaskResumed == 0x1)
              {
                elapsedTime = AUDIOPLAYER_GetProgress();
                if(getCurrentSong() != FileList.FileIndex)
                {
                  tracks[getCurrentSong()].selected = false;

                  setCurrentSong(FileList.FileIndex);
                  tracks[getCurrentSong()].selected = true;
                  songLength = tracks[FileList.FileIndex].trackLength;
                  modelListener->setMusicDuration(songLength);
                }

                songDuration = elapsedTime;
                modelListener->updateMusicDuration(elapsedTime);
                GUITaskResumed = 0x0;
              }
              else
#endif
              {
                 songDuration++;
                 modelListener->updateMusicDuration(songDuration);
              }

            }
            tickCnt = 0;
        }
        else
        {
            tickCnt++;
        }
    }
#if !defined(SIMULATOR) && defined(FUNCTIONAL_PART_AVAILABLE) && defined(PM_SUPPORT)
    if (psCountDown)
    {
        if (psTicks == TICK_COUNT)
        {
            psCountDownCnt--;
            modelListener->psCountUpdate(psCountDownCnt);
            if (psCountDownCnt == 0)
            {
                cancelPowerSave();
                osMessagePut ( LPModeEvent, AUDIOLP_ENTER_LPMODE, 0);
            }
            psTicks = 0;
        }
        else
        {
            psTicks++;
        }
    }
#endif
}

void Model::cancelPowerSave()
{
    psCountDown = false;
}

void Model::powerSavePressed()
{
    psCountDown = true;
    psTicks = 0;
    psCountDownCnt = 5;
}

void Model::setMusicVolume(int volume)
{
#if !defined(SIMULATOR) && defined(FUNCTIONAL_PART_AVAILABLE)
  if(AUDIOPLAYER_GetState() == AUDIOPLAYER_PLAY)
  {

    AUDIOPLAYER_SetVolume(volume);
  }
#endif
   musicVolume = volume;
}

int Model::getMusicVolume()
{
    return musicVolume;
}

void Model::setSongLength(int length)
{
    songLength = length;
}

int Model::getSongLength()
{
    return songLength;
}

void Model::setSongDuration(int duration)
{
  songDuration = duration;
#if !defined(SIMULATOR) && defined(FUNCTIONAL_PART_AVAILABLE)
// AUDIOPLAYER_SeekToTime(duration);
#endif
}

int Model::getSongDuration()
{
    return songDuration;
}

void Model::setPlaySong(bool play)
{
#if !defined(SIMULATOR) && defined(FUNCTIONAL_PART_AVAILABLE)
  if(play)
  {
    if(AUDIOPLAYER_GetState() == AUDIOPLAYER_STOP)
    {
      FileList.FileIndex = getCurrentSong();
      AUDIOPLAYER_Play(FileList.file[FileList.FileIndex].full_name,getMusicVolume());
      setSongDuration(songDuration);
    }
    else if(AUDIOPLAYER_GetState() == AUDIOPLAYER_PAUSE)
    {
      AUDIOPLAYER_Resume();
      if(elapsedTime != songDuration)
      {
        setSongDuration(songDuration);
        elapsedTime = songDuration;
      }
    }
  }
  else
  {
    if(AUDIOPLAYER_GetState() == AUDIOPLAYER_PLAY)
    {
      AUDIOPLAYER_Pause();
      elapsedTime = songDuration;
    }
  }
#endif
  playSong = play;
}

bool Model::getPlaySong()
{
    return playSong;
}

void Model::setTrackLength(int length, int id)
{
    tracks[id].trackLength = length;
}

int Model::getTrackLength(int id)
{
    return tracks[id].trackLength;
}

void Model::setTrackName(touchgfx::Unicode::UnicodeChar* name, int id)
{
	touchgfx::Unicode::strncpy(tracks[id].songName, name, MAX_SONG_NAME_LENGTH);
}

void Model::getTrackName(touchgfx::Unicode::UnicodeChar* name, int id)
{
	touchgfx::Unicode::strncpy(name, tracks[id].songName, MAX_SONG_NAME_LENGTH);
}

void Model::setSelected(bool selected, int id)
{
    tracks[id].selected = selected;
    if (selected)
    {
        setCurrentSong(id);
        songLength = tracks[id].trackLength;
        songDuration = 0;
        tickCnt = 0;

        modelListener->updateMusicDuration(songDuration);
        modelListener->setMusicDuration(songLength);
#if !defined(SIMULATOR) && defined(FUNCTIONAL_PART_AVAILABLE)
      if(AUDIOPLAYER_GetState() != AUDIOPLAYER_STOP)
      {
        AUDIOPLAYER_Stop();
      }
      if(getPlaySong())
      {
        FileList.FileIndex = id;
        AUDIOPLAYER_Play(FileList.file[FileList.FileIndex].full_name,getMusicVolume());
      }
#endif
    }
}

bool Model::getSelected(int id)
{
    return tracks[id].selected;
}

void Model::setCurrentSong(int song)
{
    currentSong = song;
}

int Model::getCurrentSong()
{
    return currentSong;
}

void Model::setRepeat(bool set)
{
    repeat = set;
}

bool Model::getRepeat()
{
    return repeat;
}

void Model::setShuffle(bool set)
{
    shuffle = set;
}

bool Model::getShuffle()
{
    return shuffle;
}

void Model::returnToMenuLanucher()
{
#if !defined(SIMULATOR)
  HAL_NVIC_SystemReset();
#endif
}

void Model::setNumberOfSongs(int number)
{
  numberOfSongs = number;
}

void Model::noMediaFileDetected(bool filesDetected)
{
    modelListener->noMediaFilesDetected(filesDetected);
}

void Model::audioPlayerInit()
{
#if !defined(SIMULATOR) && defined(FUNCTIONAL_PART_AVAILABLE)
  AUDIOPLAYER_Init(musicVolume);
#endif
}

void Model::setTrackList()
{
#if !defined(SIMULATOR) /*&& defined(FUNCTIONAL_PART_AVAILABLE)*/
  for (int i = 0; i < FileList.ptr; i++)
  {
    touchgfx::Unicode::UnicodeChar temp[30];
    touchgfx::Unicode::fromUTF8(FileList.file[i].name, temp, 30);
    setTrackName(temp, i);
    setTrackLength(FileList.file[i].length, i);
  }
  setNumberOfSongs(FileList.ptr);
#endif
}
