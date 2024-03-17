# --------------------------------------------------------------------------
# Build variables
#

LIBSYSTEM_SOURCES_DIR := $(LIBSYSTEM_PROJECT_DIR)/Sources

LIBSYSTEM_C_SOURCES := $(wildcard $(LIBSYSTEM_SOURCES_DIR)/*.c)
LIBSYSTEM_ASM_SOURCES := $(wildcard $(LIBSYSTEM_SOURCES_DIR)/*.s)

LIBSYSTEM_OBJS := $(patsubst $(LIBSYSTEM_SOURCES_DIR)/%.c, $(LIBSYSTEM_OBJS_DIR)/%.o, $(LIBSYSTEM_C_SOURCES))
LIBSYSTEM_DEPS := $(LIBSYSTEM_OBJS:.o=.d)
LIBSYSTEM_OBJS += $(patsubst $(LIBSYSTEM_SOURCES_DIR)/%.s, $(LIBSYSTEM_OBJS_DIR)/%.o, $(LIBSYSTEM_ASM_SOURCES))

LIBSYSTEM_C_INCLUDES := -I$(LIBSYSTEM_HEADERS_DIR) -I$(LIBSYSTEM_SOURCES_DIR)
LIBSYSTEM_ASM_INCLUDES := -I$(LIBSYSTEM_HEADERS_DIR) -I$(LIBSYSTEM_SOURCES_DIR)

#LIBSYSTEM_GENERATE_DEPS = -deps -depfile=$(patsubst $(LIBSYSTEM_OBJS_DIR)/%.o,$(LIBSYSTEM_OBJS_DIR)/%.d,$@)
LIBSYSTEM_GENERATE_DEPS := 
LIBSYSTEM_CC_DONTWARN :=


# --------------------------------------------------------------------------
# Build rules
#

.PHONY: clean-libsystemc $(LIBSYSTEM_OBJS_DIR)


build-libsystem: $(LIBSYSTEM_FILE)

$(LIBSYSTEM_OBJS): | $(LIBSYSTEM_OBJS_DIR)

$(LIBSYSTEM_OBJS_DIR):
	$(call mkdir_if_needed,$(LIBSYSTEM_OBJS_DIR))


$(LIBSYSTEM_FILE): $(LIBSYSTEM_OBJS)
	@echo Making libsystem.a
	$(LIBTOOL) create $@ $^


-include $(LIBSYSTEM_DEPS)

$(LIBSYSTEM_OBJS_DIR)/%.o : $(LIBSYSTEM_SOURCES_DIR)/%.c
	@echo $<
	@$(CC) $(USER_CC_CONFIG) $(CC_OPT_SETTING) $(CC_GEN_DEBUG_INFO) $(CC_PREPROC_DEFS) $(LIBSYSTEM_C_INCLUDES) $(LIBSYSTEM_CC_DONTWARN) $(LIBSYSTEM_GENERATE_DEPS) -o $@ $<

$(LIBSYSTEM_OBJS_DIR)/%.o : $(LIBSYSTEM_SOURCES_DIR)/%.s
	@echo $<
	@$(AS) $(USER_ASM_CONFIG) $(LIBSYSTEM_ASM_INCLUDES) -o $@ $<


clean-libsystem:
	$(call rm_if_exists,$(LIBSYSTEM_OBJS_DIR))
