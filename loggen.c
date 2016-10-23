/*
  loggen v1.0 - by Unspongeful (https://metricpanda.com)
  Public Domain
*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define FilenameLength 12
#define ARRAY_SIZE(X) (sizeof(X)/sizeof(X[0]))

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef enum LogGenerationMode
{
  LogGenerationMode_BitFlags,
  LogGenerationMode_BitGroups,
} LogGenerationMode;

typedef struct LogGroup
{
  const char* Name;
  const char** Labels;
  u32 LabelCount;
  u16 MaxLabelLength;
  u8 MaxBit;
  u8 MinBit;
} LogGroup;

int GenerateLogger(const char* Prefix, LogGenerationMode Mode, LogGroup* Groups, int GroupCount);

int main(int argc, char** argv)
{
  //NOTE: Defaults
  LogGenerationMode GenerationMode = LogGenerationMode_BitFlags;
  const char* Prefix = "LOG_";

  // NOTE: Example 1
#if 1
  const char* Priorities[] =
  {
    "VERBOSE",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
  };
  const char* Subsystems[] =
  {
    "PLATFORM",
    "AUDIO",
    "VIDEO",
    "NETWORK",
  };
  const char* Subcategories[] =
  {
    "INIT",
    "SHUTDOWN",
    "ACTIVATE",
    "DEACTIVATE",
  };
  LogGroup Groups[] =
  {
    {.Name = "Priorities", .Labels = Priorities, .LabelCount = ARRAY_SIZE(Priorities)},
    {.Name = "Subsystems", .Labels = Subsystems, .LabelCount = ARRAY_SIZE(Subsystems)},
    {.Name = "Subcategories", .Labels = Subcategories, .LabelCount = ARRAY_SIZE(Subcategories)},
  };
#else
  //NOTE: Example 2
  const char* Verbosity[] =
  {
    "Verbose",
    "Debug",
    "Error",
  };
  LogGroup Groups[] =
  {
    {.Name = "Verbosity", .Labels = Verbosity, .LabelCount = ARRAY_SIZE(Verbosity)},
  };
#endif
  int Result = GenerateLogger(Prefix, GenerationMode, Groups, ARRAY_SIZE(Groups));
  return Result;
}




/////////////////////////////////////////



#define FAIL 1
#define SUCCESS 0
#define W(...) fprintf(stdout, __VA_ARGS__)
#define MAX(X, Y) (X) > (Y) ? X : Y

static u32 Log2(u32 Value)
{
  u32 Result = 0;
  while(Value >>= 1)
  {
    Result++;
  }
  return Result;
}

static u32 ToUppercase(char* Output, int MaxLength, const char* Value)
{
  char* Start = Output;
  while(*Value && MaxLength-- > 0)
  {
    char C = *Value++;
    *Output++ = C >= 'a' &&  C <= 'z' ? (char)(C - 32) : C;
  }
  *Output = '\0';
  u32 Length = (u32)(Output-Start);
  return Length;
}

int GenerateLogger(const char* Prefix, LogGenerationMode Mode, LogGroup* Groups, int GroupCount)
{
  u32 LongestLabel = 0;
  u32 LongestGroupName = 0;
  u32 PrefixLength = 0;
  if (!Prefix)
  {
    Prefix = "";
  }
  PrefixLength = (u32)strlen(Prefix);

  //NOTE: Prep groups
  {
    u8 MinGroupBit = 0;
    for (int GroupIndex = 0; GroupIndex < GroupCount; ++GroupIndex)
    {
      LogGroup* Group = Groups + GroupIndex;
      if (!Group->Name)
      {
        fprintf(stderr, "Name not specified for group: %i", GroupIndex);
        return FAIL;
      }

      if (!Group->Labels)
      {
        fprintf(stderr, "No labels specified for group: %i", GroupIndex);
        return FAIL;
      }

      if (!Group->LabelCount)
      {
        fprintf(stderr, "Label count specified for group: %i", GroupIndex);
        return FAIL;
      }

      if (Mode == LogGenerationMode_BitFlags)
      {
        u32 MaxBit = Group->LabelCount + MinGroupBit;
        if (MaxBit > 31)
        {
          fprintf(stderr, "Too many categories specified (max 31). \n"
                          "Consider using BitGroup generation mode (-h for more information)");
          return FAIL;
        }
        Group->MaxBit = (u8)MaxBit;
      }
      else
      {
        Group->MaxBit = (u8)Log2(Group->LabelCount << MinGroupBit)+1;
      }
      Group->MinBit = MinGroupBit;
      MinGroupBit = Group->MaxBit;
      LongestGroupName = MAX(LongestGroupName, (u32)strlen(Group->Name));
      for (u32 LabelIndex = 0; LabelIndex < Group->LabelCount; ++LabelIndex)
      {
        u32 LabelLength = (u32)strlen(Group->Labels[LabelIndex]);
        LongestLabel = MAX(LongestLabel, LabelLength);
        Group->MaxLabelLength = MAX((u8)LabelLength, Group->MaxLabelLength);
      }
    }
  }

  //NOTE: Ascii art time!
  {
    char Row[256];
    u32 Offset = 33;
    W("/*\n");
    for (int GroupIndex = GroupCount-1; GroupIndex >= 0; --GroupIndex)
    {
      LogGroup* Group = Groups + GroupIndex;
      u32 StartOfWord = (Offset+1) - Group->MaxBit;
      memset(Row, ' ', StartOfWord);
      Row[StartOfWord] = '\0';
      Row[StartOfWord-1] = '-';
      Row[StartOfWord-2] = '+';
      for (int SubGroupIndex = GroupIndex + 1; SubGroupIndex < GroupCount; ++SubGroupIndex)
      {
        LogGroup* SubGroup = Groups + SubGroupIndex;
        Row[Offset-1-SubGroup->MaxBit] = '|';
      }
      const char* PrimaryGroupNotice = "";
      if (GroupIndex == 0 && Mode == LogGenerationMode_BitGroups)
      {
        PrimaryGroupNotice = " <-- Used for global verbosity";
      }
      W("  %s%s (bits %i-%i)%s\n", Row, Group->Name, Group->MinBit, Group->MaxBit, PrimaryGroupNotice);
    }
    memset(Row, ' ', Offset);
    Row[Offset] = '\0';
    for (int GroupIndex = 0; GroupIndex < GroupCount; ++GroupIndex)
    {
      LogGroup* Group = Groups + GroupIndex;
      Row[Offset-Group->MaxBit] = '|';
    }
    W(" %s\n", Row);
    memset(Row, ' ', Offset);
    Row[Offset] = '\0';
    for (int GroupIndex = 0; GroupIndex < GroupCount; ++GroupIndex)
    {
      LogGroup* Group = Groups + GroupIndex;
      Row[Offset-Group->MaxBit] = 'v';
    }
    W(" %s\n", Row);
    W("  +--------------------------------+\n");
    W("  |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|\n");
    W("  +--------------------------------+\n");
    W("   ^ bit 32                       ^ bit 0\n");
    W("*/\n\n");
  }

  W("extern int GlobalVerbosity;\n\n");

  //NOTE: Bit flags
  {
    u32 BaseOffset = 0;
    W("enum\n{\n");
    const char* NewlineBeforeSection = "";
    for (int GroupIndex = 0; GroupIndex < GroupCount; ++GroupIndex)
    {
      LogGroup* Group = Groups + GroupIndex;
      W("%s  //NOTE: %s\n", NewlineBeforeSection, Group->Name);
      for (u32 LabelIndex = 0; LabelIndex < Group->LabelCount; ++LabelIndex)
      {
        u32 Value;
        if (Mode == LogGenerationMode_BitFlags)
        {
          Value = LabelIndex + Group->MinBit;
          W("  %s%-*s = 1 << %2i,\n", Prefix, LongestLabel+PrefixLength, Group->Labels[LabelIndex], Value);
        }
        else
        {
          Value = (LabelIndex+BaseOffset) << Group->MinBit;
          W("  %s%-*s = 0x%08x,\n", Prefix, LongestLabel+PrefixLength, Group->Labels[LabelIndex], Value);
        }
      }
      BaseOffset = 1;
      NewlineBeforeSection = "\n";
    }

    W("\n  //NOTE: Masks\n");
    u32 PreviousGroupMask = 0;
    for (int GroupIndex = 0; GroupIndex < GroupCount; ++GroupIndex)
    {
      LogGroup* Group = Groups + GroupIndex;
      u32 GroupMask = (1 << Group->MaxBit)-1;
      char Uppercase[256];
      ToUppercase(Uppercase, sizeof(Uppercase), Group->Name);
      u32 LabelLength = LongestGroupName+PrefixLength;
      W("  %s%-*s = 0x%08x,  /* (bits %02i-%02i) */\n", Prefix, LabelLength, Uppercase, GroupMask & ~PreviousGroupMask, Group->MinBit, Group->MaxBit-1);
      PreviousGroupMask = GroupMask;
    }
    u32 LabelLength = (LongestGroupName+PrefixLength);
    W("  %s%-*s = 0x7fffffff\n", Prefix, LabelLength, "EVERYTHING");
    W("};\n\n");
  }

  //NOTE: Log macro
  {
    char Uppercase[256];
    char StringFormats[256];
    u32 Advance = 0;
    for (int GroupIndex = 1; GroupIndex < GroupCount; ++GroupIndex)
    {
      Advance += (u32)snprintf(StringFormats + Advance, sizeof(StringFormats)-Advance, "%%s");
    }
    ToUppercase(Uppercase, sizeof(Uppercase), Groups[0].Name);
    W("#define Log(Flags, Format, ...) \\\n");
    W("{ \\\n");
    for (int GroupIndex = 0; GroupIndex < GroupCount; ++GroupIndex)
    {
      LogGroup* Group = Groups + GroupIndex;
      for (u32 LabelIndex = 0; LabelIndex < Group->LabelCount; ++LabelIndex)
      {
        W("  int %-*s = %s%s; \\\n", LongestLabel, Group->Labels[LabelIndex], Prefix, Group->Labels[LabelIndex]);
      }
    }
    if (Mode == LogGenerationMode_BitFlags)
    {
      W("  if ((Flags) & GlobalVerbosity) { \\\n");
    }
    else
    {
      W("  if (((Flags) & %s%s) >= GlobalVerbosity) { \\\n", Prefix, Uppercase);
    }
    W("    printf(\"%%s%s %%%i.%is:%%4i \" Format \"\\n\", \\\n", StringFormats, FilenameLength, FilenameLength);
    W("           ");
    for (int GroupIndex = 0; GroupIndex < GroupCount; ++GroupIndex)
    {
      W("%s%sLabel(Flags), ", Prefix, Groups[GroupIndex].Name);
    }
    W("\\\n           __FILE__, __LINE__, ##__VA_ARGS__); \\\n");
    W("  } \\\n");
    W("}\n");
  }

  //NOTE: Log labels
  {
    char Row[256];
    for (int GroupIndex = 0; GroupIndex < GroupCount; ++GroupIndex)
    {
      LogGroup* Group = Groups + GroupIndex;
      memset(Row, ' ', Group->MaxLabelLength);
      Row[Group->MaxLabelLength] = '\0';
      char Uppercase[256];
      ToUppercase(Uppercase, sizeof(Uppercase), Group->Name);
      W("static inline const char* %s%sLabel(int Flags)\n{\n", Prefix, Group->Name);
      W("  switch (Flags & %s%s)\n", Prefix, Uppercase);
      W("  {\n");
      for (u32 LabelIndex = 0; LabelIndex < Group->LabelCount; ++LabelIndex)
      {
        u16 LabelLength = (u16)strlen(Group->Labels[LabelIndex]);
        W("    case %s%s: %*sreturn \"[%-*s]\";\n", Prefix, Group->Labels[LabelIndex], (PrefixLength+Group->MaxLabelLength)-LabelLength, "", Group->MaxLabelLength, Group->Labels[LabelIndex]);
      }
      W("  }\n");
      W("  return \"[%s]\";\n", Row);
      W("}\n\n");
    }
  }
  return SUCCESS;
}
