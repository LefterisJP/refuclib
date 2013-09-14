class BuildMsgError(Exception):
    def __init__(self, msg):
        self.msg = msg

    def __str__(self):
        return "Error in build message: {}".format(self.msg)


msg_intro = "**Refu clib build system - "


def build_msg(msg, msg_type="Error", env=None):
    """
       Prints a build message for Refu C library building.
       msg - The message to print
       env - If the message type is Info you should also provice an
            environment object so that we can determine if Scons was run
            with --debug=explain
       msg_type - The message type. Can be either 'Error', 'Info' or 'Warning'
    """
    if msg_type == "Error":
        print("{}[ERROR]: {}".format(msg_intro, msg))
    elif msg_type == "Info":
        if env is None:
            raise BuildMsgError("Did not provide an environment for an "
                                "Info build message and can't determine "
                                "if SCons --debug=explain is given")
        if env.GetOption('debug') == ['explain']:
            print("{}[INFO]: {}".format(msg_intro, msg))
    elif msg_type == "Warning":
        print("{}[WARNING]: {}".format(msg_intro, msg))
    else:
        raise BuildMsgError("Unrecognized message type \"{}\" "
                            "provided".format(msg_type))
