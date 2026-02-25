/**
  **************************************************************************
  * @file     at_surf_f437_board_player.c
  * @version  v2.0.0
  * @date     2020-11-02
  * @brief    music player.
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to 
  * download from Artery official website is the copyrighted work of Artery. 
  * Artery authorizes customers to use, copy, and distribute the BSP 
  * software and its related documentation for the purpose of design and 
  * development in conjunction with Artery microcontrollers. Use of the 
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPretENTATIONS OF ANY KIND. ARTERY EXPretSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPretS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPretENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
  
#include "at_surf_f437_board_audio.h"
#include "music_player.h"
#include "lvgl.h"

#define MUSIC_DIR   "A:/MUSIC"

uint8_t music_read_buffer[MP3_READBUF_SIZE];
int16_t music_output_buffer[MP3_DECODE_BUFFER_SIZE];


FIL file_music;
uint8_t name_buf[FF_LFN_BUF + 1];
uint16_t index = 0; 
audio_type audio_info; 

/**
  * @brief  compare whether the valus of buffer 1 and buffer 2 are equal.
  * @param  buffer1: buffer 1 addrets.
  *         buffer2: buffer 2 addrets.
  * @retval ERROR: equal.
  *         SUCCESS: unequal.
  */
error_status buf_cmp(uint8_t* buffer1, uint8_t* buffer2, uint32_t len)
{
  uint32_t i;
  
  for(i = 0; i < len; i++)
  {
    if(buffer1[i] != buffer2[i])
    {
      return ERROR;
    }
  }

  return SUCCESS;
}

/**
  * @brief  find equal strings in buf.
  * @param  buf: buffer to be searched.
  *         find_str: strings.
  * @retval ERROR: target strings not found.
  *         SUCCESS: target strings found successfull.
  */
error_status str_find(const char *buf, const char *find_str)
{
  uint32_t i;
  uint32_t buf_len, find_len;

  buf_len = strlen(buf);
  find_len = strlen(find_str);
  
  if(buf_len < find_len)
  {
    return ERROR;
  }

  buf_len = buf_len - find_len + 1;
  
  for(i = 0; i < buf_len; i++)
  {
    if(buf_cmp((uint8_t *)&buf[i], (uint8_t *)find_str, find_len) == SUCCESS)
    {
      return SUCCESS;
    }
  }
  
  return ERROR;
}

/**
  * @brief  get the number of valid music file_mp3.
  * @param  path: music file mp3 path.
  *         info: file_mp3 information.
  * @retval number of valid music file mp3 found.
  */
music_format_type music_file_format_get(char *name)
{    
  /* find files in mp3 format */
  if((str_find((const char *)name, ".mp3") == SUCCESS) || (str_find((const char *)name, ".MP3") == SUCCESS))
  {
    return MUSIC_MP3;
  } 
  
  if((str_find((const char *)name, ".wav") == SUCCESS) || (str_find((const char *)name, ".WAV") == SUCCESS))
  {
    return MUSIC_WAV;
  }

  if((str_find((const char *)name, ".ape") == SUCCESS) || (str_find((const char *)name, ".APE") == SUCCESS))
  {
    return MUSIC_APE;
  }
  
  if((str_find((const char *)name, ".flac") == SUCCESS) || (str_find((const char *)name, ".FLAC") == SUCCESS))
  {
    return MUSIC_FLAC;
  }
  
  return MUSIC_ERROR;
}


/**
  * @brief  get the number of valid music file_mp3.
  * @param  path: music file mp3 path.
  *         info: file_mp3 information.
  * @retval number of valid music file mp3 found.
  */
uint16_t mp3_file_number_get(const char * path)
{    
  FRESULT ret = 0; 
  lv_fs_dir_t   dir;
  uint16_t cnt = 0;
  char fn[100];

  /* open the specified path */
  ret = lv_fs_dir_open(&dir,path);

  if(ret == FR_OK)
  {
    while(1)
    {     
     /* read the file infromation under the path */ 
     ret =  lv_fs_dir_read (&dir, fn);;
      if((ret != FR_OK) || (fn[0]==0))
      {
        break;
      }
      
      /* find files in mp3 format */
      if((str_find(fn, ".mp3")  == SUCCESS) || 
         (str_find(fn, ".MP3")  == SUCCESS) || 
         (str_find(fn, ".wav")  == SUCCESS) || 
         (str_find(fn, ".WAV")  == SUCCESS) ||
         (str_find(fn, ".ape")  == SUCCESS) || 
         (str_find(fn, ".APE")  == SUCCESS) || 
         (str_find(fn, ".flac") == SUCCESS) || 
         (str_find(fn, ".FLAC") == SUCCESS))
      {
        cnt++;
      }
    }
  }
  
  /* close path */
  lv_fs_dir_close(&dir);

  return cnt;
}

/**
  * @brief  get the music file_mp3 names in the specified order.
  * @param  path: music file_mp3 path.
  *         info: music file_mp3 infromation.
  *         id: specified order.
  * @retval ERROR: specified order music not found.
  *         SUCCESS: specified order music found successfull.
  */
error_status mp3_get_name(uint8_t *path, char *info, uint16_t id)
{    
  uint8_t ret = 0;
  uint16_t cnt = 0;
  lv_fs_dir_t   dir;      
//  char fn[100];
  /* open the specified path */
  ret = lv_fs_dir_open(&dir,(const TCHAR*)path);

  if(ret == FR_OK)
  {
    while(1)
    {
      /* read the file infromation under the path */
      ret = lv_fs_dir_read(&dir, info); 
      
      if((ret != FR_OK) || (info[0]==0))
      {
        return ERROR;
      }
      
      /* find files in mp3 format */
      if((str_find(info, ".mp3")  == SUCCESS) || 
         (str_find(info, ".MP3")  == SUCCESS) || 
         (str_find(info, ".wav")  == SUCCESS) || 
         (str_find(info, ".WAV")  == SUCCESS) ||
         (str_find(info, ".ape")  == SUCCESS) || 
         (str_find(info, ".APE")  == SUCCESS) || 
         (str_find(info, ".flac") == SUCCESS) || 
         (str_find(info, ".FLAC") == SUCCESS))
      {
        if(id == cnt)
        {
          lv_fs_dir_close(&dir);
          return SUCCESS; 
        }
        
        cnt++;
      }
    }
  }
  lv_fs_dir_close(&dir);
  return ERROR;
}

/**
  * @brief  get play volume.
  * @param  none.
  * @retval volume.
  */
uint16_t volume_value_get(void)
{
  static uint16_t last = 0;
  uint16_t vol = 0;
  
  /* software filtering */
  last = last * 0.6 + VR_ADC->odt * 0.4;
  
  vol = last / 40.96 + 0.5;
  
  if(vol > 100)
  {
    vol = 100;
  }
  
  return vol;
}

/**
  * @brief  calculate play volume.
  * @param  none.
  * @retval none.
  */
void volume_value_set(void)
{
  static uint16_t vol_last = 0xFFFF;
  uint16_t vol;

  /* calculate play volume */
  vol = volume_value_get() * 1.59 + 96;
  
  /* the volume value changes */
  if(vol_last != vol)
  {
    /* set volume */
    audio_spk_volume_set(vol, vol);
    
    vol_last = vol;
  }
}



/**
  * @brief  play music.
  * @param  none. 
  * @retval none
  */
void music_play(void)
{

  uint16_t mp3_number;
  char music_name[50];
  
  /* get the number of music files */
  mp3_number = mp3_file_number_get(MUSIC_DIR);
  if(mp3_number)
  printf("mp3_number=%d\r\n",mp3_number);
  else
  printf("music file not found\r\n");    
 
       
  /* set volume */
//  volume_value_set();
  
//  while(1)
//  {
    /* play mp3 */
    while(mp3_get_name((uint8_t *)MUSIC_DIR, music_name, index) == SUCCESS)
    {
      printf("music_name:%s  %d\r\n",music_name,index++);
//      printf()
//      strcpy((char *)name_buf,"A:/MUSIC/");
//      strcat((char *)name_buf,(const char*)file_music_info.fname); 

//      /* display the number of music files */
//      sprintf((char *)buf, "%02d/%02d", index + 1, mp3_number);
//      lcd_string_show(10, 110, 200, 24, 24, &music_name);    

//      /* show music name */
//      lcd_fill(10, 145, 319, 160 + 24, BLACK);
//      lcd_string_show(10, 145, 319, 24, 24, (uint8_t *)file_music_info.fname);  
//      
//      lcd_fill(10, 240, 319, 470, BLACK);
//      
      audio_info.music_type = music_file_format_get(music_name);
      
      /* play music */
      switch(audio_info.music_type)
      {
        
          case  MUSIC_ERROR:
          break;
        /* key 1 down */  
        case MUSIC_MP3:
          printf("MUSIC_MP3\r\n");
//          mp3_song_play(name_buf);              
//          break;
//        case MUSIC_WAV:
//          wav_song_play(name_buf);                
//          break;
        case MUSIC_FLAC:
                    
          break;
        case MUSIC_APE:
                       
          break;
        
        default:
          break;
      }
      
//      /* music switch */
//      switch(audio_info.key)
//      {
//        /* key 1 down */  
//        case KEY_1:
//          
//          if(index)
//          {
//            index--;  
//          }
//          else
//          {
//            index = mp3_number - 1; 
//          }
//          break;

//        /* key 2 down */  
//        case KEY_2:
//        case NO_KEY:
//          
//          index++;
//        
//          if(index >= mp3_number)
//          {
//            index = 0;
//          }
//          break;
//        
//        default:
//          break;
//      }
//    }
  } 
}

