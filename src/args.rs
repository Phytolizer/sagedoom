pub(crate) fn parm_exists(args: &[String], parm: impl AsRef<str>) -> bool {
    // not using `contains` because it requires `parm` be &String
    args.iter().any(|a| a == parm.as_ref())
}

pub(crate) fn check_parm(args: &[String], parm: impl AsRef<str>) -> Option<usize> {
    args.iter().position(|a| a == parm.as_ref())
}
