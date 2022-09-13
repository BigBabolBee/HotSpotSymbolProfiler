#! /bin/bash
set -e

PID=$1
ATTACH_JAR=attach-myagent.jar
JPA_DIR="$(cd "$(dirname "$0")" && pwd -P)"/..
ATTACH_JAR_PATH=${JPA_DIR}/build/out/${ATTACH_JAR}
JITCACHE_FILE=/tmp/jitcache-${PID}.dump

if [[ $(uname) == 'Linux' ]]; then
    LINUX=1;
else
    LINUX=2;
fi

if [[ "$LINUX" == "1" ]]; then
  if [ ! -d /proc/$PID ]; then
    echo "PID $PID not found"
    exit 1
  fi
  TARGET_UID=$(awk '/^Uid:/{print $2}' /proc/$PID/status)
  TARGET_GID=$(awk '/^Gid:/{print $2}' /proc/$PID/status)
fi


if [ -z "$JAVA_HOME" ];then
    if [[ "${LINUX}" == "1" ]]; then
        JAVA_HOME=/usr/lib/jvm/default-java

        [ -d "$JAVA_HOME" ] || JAVA_HOME=/etc/alternatives/java_sdk
    else
        JAVA_HOME=$(/usr/libexec/java_home -v 1.8)
    fi
fi
[ -d "$JAVA_HOME" ] || (echo "JAVA_HOME directory at '$JAVA_HOME' does not exist." && false)

if [[ "$LINUX" == "1" ]]; then
  # sudo rm ${JITCACHE_FILE} -f
  (cd ${JPA_DIR}/build/out && sudo -u \#$TARGET_UID -g \#$TARGET_GID $JAVA_HOME/bin/java -Xcheck:jni -cp $ATTACH_JAR_PATH:$JAVA_HOME/lib/tools.jar AttachAgent $PID)
  # sudo chown root:root ${JITCACHE_FILE}
else
  rm -f ${JITCACHE_FILE}
  (cd ${JPA_DIR}/build/out && $JAVA_HOME/bin/java -Xcheck:jni -cp $ATTACH_JAR_PATH:$JAVA_HOME/lib/tools.jar net.virtualvoid.javaagent.AttachAgent $PID)
fi
