package md5486166faafae792ea15d9ebdd58e0669;


public class SvgImageRenderer
	extends md51558244f76c53b6aeda52c8a337f2c37.ViewRenderer_2
	implements
		mono.android.IGCUserPeer
{
/** @hide */
	public static final String __md_methods;
	static {
		__md_methods = 
			"";
		mono.android.Runtime.register ("SVG.Forms.Plugin.Droid.SvgImageRenderer, SVG.Forms.Plugin.Android", SvgImageRenderer.class, __md_methods);
	}


	public SvgImageRenderer (android.content.Context p0)
	{
		super (p0);
		if (getClass () == SvgImageRenderer.class)
			mono.android.TypeManager.Activate ("SVG.Forms.Plugin.Droid.SvgImageRenderer, SVG.Forms.Plugin.Android", "Android.Content.Context, Mono.Android", this, new java.lang.Object[] { p0 });
	}


	public SvgImageRenderer (android.content.Context p0, android.util.AttributeSet p1)
	{
		super (p0, p1);
		if (getClass () == SvgImageRenderer.class)
			mono.android.TypeManager.Activate ("SVG.Forms.Plugin.Droid.SvgImageRenderer, SVG.Forms.Plugin.Android", "Android.Content.Context, Mono.Android:Android.Util.IAttributeSet, Mono.Android", this, new java.lang.Object[] { p0, p1 });
	}


	public SvgImageRenderer (android.content.Context p0, android.util.AttributeSet p1, int p2)
	{
		super (p0, p1, p2);
		if (getClass () == SvgImageRenderer.class)
			mono.android.TypeManager.Activate ("SVG.Forms.Plugin.Droid.SvgImageRenderer, SVG.Forms.Plugin.Android", "Android.Content.Context, Mono.Android:Android.Util.IAttributeSet, Mono.Android:System.Int32, mscorlib", this, new java.lang.Object[] { p0, p1, p2 });
	}

	private java.util.ArrayList refList;
	public void monodroidAddReference (java.lang.Object obj)
	{
		if (refList == null)
			refList = new java.util.ArrayList ();
		refList.add (obj);
	}

	public void monodroidClearReferences ()
	{
		if (refList != null)
			refList.clear ();
	}
}
